/*
 * GM20B Fifo
 *
 * Copyright (c) 2014-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <nvgpu/timers.h>
#include <nvgpu/log.h>
#include <nvgpu/atomic.h>
#include <nvgpu/barrier.h>
#include <nvgpu/mm.h>
#include <nvgpu/enabled.h>
#include <nvgpu/io.h>
#include <nvgpu/bug.h>
#include <nvgpu/gk20a.h>
#include <nvgpu/channel.h>
#include <nvgpu/top.h>
#include <nvgpu/engines.h>

#include "gk20a/fifo_gk20a.h"
#include "fifo_gm20b.h"

#include <nvgpu/hw/gm20b/hw_ram_gm20b.h>
#include <nvgpu/hw/gm20b/hw_fifo_gm20b.h>
#include <nvgpu/hw/gm20b/hw_pbdma_gm20b.h>

void gm20b_fifo_tsg_verify_status_ctx_reload(struct channel_gk20a *ch)
{
	struct gk20a *g = ch->g;
	struct tsg_gk20a *tsg = &g->fifo.tsg[ch->tsgid];
	struct channel_gk20a *temp_ch;
	struct nvgpu_channel_hw_state hw_state;

	/* If CTX_RELOAD is set on a channel, move it to some other channel */
	g->ops.channel.read_state(g, ch, &hw_state);
	if (hw_state.ctx_reload) {
		nvgpu_rwsem_down_read(&tsg->ch_list_lock);
		nvgpu_list_for_each_entry(temp_ch, &tsg->ch_list, channel_gk20a, ch_entry) {
			if (temp_ch->chid != ch->chid) {
				g->ops.channel.force_ctx_reload(temp_ch);
				break;
			}
		}
		nvgpu_rwsem_up_read(&tsg->ch_list_lock);
	}
}

int gm20b_fifo_init_engine_info(struct fifo_gk20a *f)
{
	struct gk20a *g = f->g;
	int ret = 0;
	enum nvgpu_fifo_engine engine_enum;
	u32 pbdma_id = U32_MAX;
	bool found_pbdma_for_runlist = false;

	f->num_engines = 0;
	if (g->ops.top.get_device_info != NULL) {
		struct nvgpu_device_info dev_info;
		struct fifo_engine_info_gk20a *info;

		ret = g->ops.top.get_device_info(g, &dev_info,
						NVGPU_ENGINE_GRAPHICS, 0);
		if (ret != 0) {
			nvgpu_err(g,
				"Failed to parse dev_info table for engine %d",
				NVGPU_ENGINE_GRAPHICS);
			return -EINVAL;
		}

		found_pbdma_for_runlist = g->ops.fifo.find_pbdma_for_runlist(f,
							dev_info.runlist_id,
							&pbdma_id);
		if (!found_pbdma_for_runlist) {
			nvgpu_err(g, "busted pbdma map");
			return -EINVAL;
		}

		engine_enum = nvgpu_engine_enum_from_type(g,
							dev_info.engine_type);

		info = &g->fifo.engine_info[dev_info.engine_id];

		info->intr_mask |= BIT32(dev_info.intr_id);
		info->reset_mask |= BIT32(dev_info.reset_id);
		info->runlist_id = dev_info.runlist_id;
		info->pbdma_id = pbdma_id;
		info->inst_id  = dev_info.inst_id;
		info->pri_base = dev_info.pri_base;
		info->engine_enum = engine_enum;
		info->fault_id = dev_info.fault_id;

		/* engine_id starts from 0 to NV_HOST_NUM_ENGINES */
		f->active_engines_list[f->num_engines] = dev_info.engine_id;
		++f->num_engines;
		nvgpu_log_info(g, "gr info: engine_id %d runlist_id %d intr_id %d "
			"reset_id %d engine_type %d engine_enum %d inst_id %d",
					dev_info.engine_id,
					dev_info.runlist_id,
					dev_info.intr_id,
					dev_info.reset_id,
					dev_info.engine_type,
					engine_enum,
					dev_info.inst_id);
	}

	ret = g->ops.fifo.init_ce_engine_info(f);

	return 0;
}

int gm20b_fifo_init_ce_engine_info(struct fifo_gk20a *f)
{
	struct gk20a *g = f->g;
	int ret = 0;
	u32 i;
	enum nvgpu_fifo_engine engine_enum;
	u32 pbdma_id = U32_MAX;
	u32 gr_runlist_id;
	bool found_pbdma_for_runlist = false;

	gr_runlist_id = nvgpu_engine_get_gr_runlist_id(g);
	nvgpu_log_info(g, "gr_runlist_id: %d", gr_runlist_id);

	if (g->ops.top.get_device_info != NULL) {
		for (i = NVGPU_ENGINE_COPY0;  i <= NVGPU_ENGINE_COPY2; i++) {
			struct nvgpu_device_info dev_info;
			struct fifo_engine_info_gk20a *info;

			ret = g->ops.top.get_device_info(g, &dev_info, i, 0);
			if (ret != 0) {
				nvgpu_err(g,
					"Failed to parse dev_info table for"
					" engine %d", i);
				return ret;
			}
			if (dev_info.engine_type != i) {
				nvgpu_log_info(g, "No entry found in dev_info "
					"table for engine_type %d", i);
				continue;
			}

			found_pbdma_for_runlist =
					g->ops.fifo.find_pbdma_for_runlist(f,
							dev_info.runlist_id,
							&pbdma_id);
			if (!found_pbdma_for_runlist) {
				nvgpu_err(g, "busted pbdma map");
				return -EINVAL;
			}

			info = &g->fifo.engine_info[dev_info.engine_id];

			engine_enum = nvgpu_engine_enum_from_type(g,
							dev_info.engine_type);

			/* GR and GR_COPY shares same runlist_id */
			if ((engine_enum == NVGPU_ENGINE_ASYNC_CE_GK20A) &&
				(gr_runlist_id == dev_info.runlist_id)) {
					engine_enum = NVGPU_ENGINE_GRCE_GK20A;
			}
			info->engine_enum = engine_enum;

			if (g->ops.top.get_ce_inst_id != NULL) {
				dev_info.inst_id = g->ops.top.get_ce_inst_id(g,
							dev_info.engine_type);
			}

			if ((dev_info.fault_id == 0U) &&
					(engine_enum == NVGPU_ENGINE_GRCE_GK20A)) {
				dev_info.fault_id = 0x1b;
			}
			info->fault_id = dev_info.fault_id;

			info->intr_mask |= BIT32(dev_info.intr_id);
			info->reset_mask |= BIT32(dev_info.reset_id);
			info->runlist_id = dev_info.runlist_id;
			info->pbdma_id = pbdma_id;
			info->inst_id  = dev_info.inst_id;
			info->pri_base = dev_info.pri_base;

			/* engine_id starts from 0 to NV_HOST_NUM_ENGINES */
			f->active_engines_list[f->num_engines] =
							dev_info.engine_id;
			++f->num_engines;
			nvgpu_log_info(g, "gr info: engine_id %d runlist_id %d "
				"intr_id %d reset_id %d engine_type %d "
				"engine_enum %d inst_id %d",
				dev_info.engine_id,
				dev_info.runlist_id,
				dev_info.intr_id,
				dev_info.reset_id,
				dev_info.engine_type,
				engine_enum,
				dev_info.inst_id);
		}
	}
	return 0;
}
