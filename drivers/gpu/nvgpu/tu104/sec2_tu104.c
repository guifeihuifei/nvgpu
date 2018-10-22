/*
 * Copyright (c) 2018, NVIDIA CORPORATION.  All rights reserved.
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

#include <nvgpu/gk20a.h>
#include <nvgpu/pmu.h>
#include <nvgpu/falcon.h>
#include <nvgpu/mm.h>
#include <nvgpu/io.h>
#include <nvgpu/timers.h>
#include <nvgpu/falcon.h>
#include <nvgpu/sec2.h>

#include "sec2_tu104.h"

#include <nvgpu/hw/tu104/hw_pwr_tu104.h>
#include <nvgpu/hw/tu104/hw_psec_tu104.h>

int tu104_sec2_reset(struct gk20a *g)
{
	nvgpu_log_fn(g, " ");

	gk20a_writel(g, psec_falcon_engine_r(),
			psec_falcon_engine_reset_true_f());
	nvgpu_udelay(10);
	gk20a_writel(g, psec_falcon_engine_r(),
			psec_falcon_engine_reset_false_f());

	nvgpu_log_fn(g, "done");
	return 0;
}

static int tu104_sec2_emem_transfer(struct gk20a *g, u32 dmem_addr, u8 *buf,
	u32 size_in_bytes, u8 port, bool is_copy_from)
{
	u32 *data = (u32 *)(void *)buf;
	u32 num_words = 0;
	u32 num_bytes = 0;
	u32 start_emem = 0;
	u32 end_emem = 0;
	u32 reg = 0;
	u32 i = 0;
	u32 emem_c_offset = 0;
	u32 emem_d_offset = 0;
	u8 max_emem_ports = (u8)psec_ememc__size_1_v();
	int status = 0;

	if (size_in_bytes == 0U) {
		nvgpu_err(g, "zero-byte copy requested");
		status = -EINVAL;
		goto exit;
	}

	if (port >= max_emem_ports) {
		nvgpu_err(g, "only %d ports supported. Accessed port=%d\n",
			max_emem_ports, port);
		status = -EINVAL;
		goto exit;
	}

	/*
	 * Get the EMEMC/D register addresses
	 * for the specified port
	 */
	emem_c_offset = psec_ememc_r(port);
	emem_d_offset = psec_ememd_r(port);

	/*
	 * EMEM is mapped at the top of DMEM VA space
	 * START_EMEM = DMEM_VA_MAX = 2^(DMEM_TAG_WIDTH + 8)
	 */
	start_emem = (u32)1U << ((u32)psec_falcon_hwcfg1_dmem_tag_width_v(
			gk20a_readl(g, psec_falcon_hwcfg1_r())) + (u32)8U);

	end_emem = start_emem +
		((u32)psec_hwcfg_emem_size_f(gk20a_readl(g, psec_hwcfg_r()))
		* (u32)256U);

	if (dmem_addr < start_emem ||
		(dmem_addr + size_in_bytes) > end_emem) {
		nvgpu_err(g, "copy must be in emem aperature [0x%x, 0x%x]",
			start_emem, end_emem);
		status = -EINVAL;
		goto exit;
	}

	/* Convert to emem offset for use by EMEMC/EMEMD */
	dmem_addr -= start_emem;

	/* Mask off all but the OFFSET and BLOCK in EMEM offset */
	reg = dmem_addr & (psec_ememc_offs_m() |
		psec_ememc_blk_m());

	if (is_copy_from) {
		/* mark auto-increment on read */
		reg |= psec_ememc_aincr_m();
	} else {
		/* mark auto-increment on write */
		reg |= psec_ememc_aincw_m();
	}

	gk20a_writel(g, emem_c_offset, reg);

	/* Calculate the number of words and bytes */
	num_words = size_in_bytes >> 2U;
	num_bytes = size_in_bytes & 0x3U;

	/* Directly copy words to emem*/
	for (i = 0; i < num_words; i++) {
		if (is_copy_from) {
			data[i] = gk20a_readl(g, emem_d_offset);
		} else {
			gk20a_writel(g, emem_d_offset, data[i]);
		}
	}

	/* Check if there are leftover bytes to copy */
	if (num_bytes > 0U) {
		u32 bytes_copied = num_words << 2U;

		reg = gk20a_readl(g, emem_d_offset);
		if (is_copy_from) {
			for (i = 0; i < num_bytes; i++) {
				buf[bytes_copied + i] = ((u8 *)&reg)[i];
			}
		} else {
			for (i = 0; i < num_bytes; i++) {
				((u8 *)&reg)[i] = buf[bytes_copied + i];
			}
			gk20a_writel(g, emem_d_offset, reg);
		}
	}

exit:
	return status;
}

int tu104_sec2_flcn_copy_to_emem(struct nvgpu_falcon *flcn,
	u32 dst, u8 *src, u32 size, u8 port)
{
	struct gk20a *g = flcn->g;

	return tu104_sec2_emem_transfer(g, dst, src, size, port, false);
}

int tu104_sec2_flcn_copy_from_emem(struct nvgpu_falcon *flcn,
	u32 src, u8 *dst, u32 size, u8 port)
{
	struct gk20a *g = flcn->g;

	return tu104_sec2_emem_transfer(g, src, dst, size, port, true);
}

static int tu104_sec2_flcn_bl_bootstrap(struct gk20a *g,
	struct nvgpu_falcon_bl_info *bl_info)
{
	struct mm_gk20a *mm = &g->mm;
	u32 data = 0;

	nvgpu_log_fn(g, " ");

	/* SEC2 Config */
	gk20a_writel(g, psec_falcon_itfen_r(),
			gk20a_readl(g, psec_falcon_itfen_r()) |
			psec_falcon_itfen_ctxen_enable_f());

	gk20a_writel(g, psec_falcon_nxtctx_r(),
			pwr_pmu_new_instblk_ptr_f(
			nvgpu_inst_block_addr(g, &mm->pmu.inst_block) >> 12U) |
			pwr_pmu_new_instblk_valid_f(1U) |
			nvgpu_aperture_mask(g, &mm->pmu.inst_block,
				pwr_pmu_new_instblk_target_sys_ncoh_f(),
				pwr_pmu_new_instblk_target_sys_coh_f(),
				pwr_pmu_new_instblk_target_fb_f()));

	data = gk20a_readl(g, psec_falcon_debug1_r());
	data |= psec_falcon_debug1_ctxsw_mode_m();
	gk20a_writel(g, psec_falcon_debug1_r(), data);

	data = gk20a_readl(g, psec_falcon_engctl_r());
	data |= (1U << 3U);
	gk20a_writel(g, psec_falcon_engctl_r(), data);

	return nvgpu_flcn_bl_bootstrap(&g->sec2_flcn, bl_info);
}

int tu104_sec2_setup_hw_and_bl_bootstrap(struct gk20a *g,
	struct hs_acr *acr_desc,
	struct nvgpu_falcon_bl_info *bl_info)
{
	u32 data = 0U;

	nvgpu_log_fn(g, " ");

	nvgpu_flcn_reset(&g->sec2_flcn);

	data = gk20a_readl(g, psec_fbif_ctl_r());
	data |= psec_fbif_ctl_allow_phys_no_ctx_allow_f();
	gk20a_writel(g, psec_fbif_ctl_r(), data);

	/* setup apertures - virtual */
	gk20a_writel(g, psec_fbif_transcfg_r(GK20A_PMU_DMAIDX_UCODE),
			psec_fbif_transcfg_mem_type_physical_f() |
			psec_fbif_transcfg_target_local_fb_f());
	gk20a_writel(g, psec_fbif_transcfg_r(GK20A_PMU_DMAIDX_VIRT),
			psec_fbif_transcfg_mem_type_virtual_f());
	/* setup apertures - physical */
	gk20a_writel(g, psec_fbif_transcfg_r(GK20A_PMU_DMAIDX_PHYS_VID),
			psec_fbif_transcfg_mem_type_physical_f() |
			psec_fbif_transcfg_target_local_fb_f());
	gk20a_writel(g, psec_fbif_transcfg_r(GK20A_PMU_DMAIDX_PHYS_SYS_COH),
			psec_fbif_transcfg_mem_type_physical_f() |
			psec_fbif_transcfg_target_coherent_sysmem_f());
	gk20a_writel(g, psec_fbif_transcfg_r(GK20A_PMU_DMAIDX_PHYS_SYS_NCOH),
			psec_fbif_transcfg_mem_type_physical_f() |
			psec_fbif_transcfg_target_noncoherent_sysmem_f());

	return tu104_sec2_flcn_bl_bootstrap(g, bl_info);
}

int tu104_sec2_queue_head(struct gk20a *g, struct nvgpu_falcon_queue *queue,
	u32 *head, bool set)
{
	u32 queue_head_size = 8;

	if (queue->id <= SEC2_NV_CMDQ_LOG_ID__LAST) {
		if (queue->index >= queue_head_size) {
			return -EINVAL;
		}

		if (!set) {
			*head = psec_queue_head_address_v(
				gk20a_readl(g, psec_queue_head_r(queue->index)));
		} else {
			gk20a_writel(g, psec_queue_head_r(queue->index),
				psec_queue_head_address_f(*head));
		}
	} else {
		if (!set) {
			*head = psec_msgq_head_val_v(
				gk20a_readl(g, psec_msgq_head_r(0U)));
		} else {
			gk20a_writel(g,
				psec_msgq_head_r(0U),
				psec_msgq_head_val_f(*head));
		}
	}

	return 0;
}

int tu104_sec2_queue_tail(struct gk20a *g, struct nvgpu_falcon_queue *queue,
	u32 *tail, bool set)
{
	u32 queue_tail_size = 8;

	if (queue->id <= SEC2_NV_CMDQ_LOG_ID__LAST) {
		if (queue->index >= queue_tail_size) {
			return -EINVAL;
		}

		if (!set) {
			*tail = psec_queue_tail_address_v(
				gk20a_readl(g, psec_queue_tail_r(queue->index)));
		} else {
			gk20a_writel(g,
				psec_queue_tail_r(queue->index),
				psec_queue_tail_address_f(*tail));
		}
	} else {
		if (!set) {
			*tail = psec_msgq_tail_val_v(
				gk20a_readl(g, psec_msgq_tail_r(0U)));
		} else {
			gk20a_writel(g, psec_msgq_tail_r(0U),
				psec_msgq_tail_val_f(*tail));
		}
	}

	return 0;
}

void tu104_sec2_msgq_tail(struct gk20a *g, struct nvgpu_sec2 *sec2,
	u32 *tail, bool set)
{
	if (!set) {
		*tail = gk20a_readl(g, psec_msgq_tail_r(0U));
	} else {
		gk20a_writel(g, psec_msgq_tail_r(0U), *tail);
	}
}

void tu104_sec2_enable_irq(struct nvgpu_sec2 *sec2, bool enable)
{
	struct gk20a *g = sec2->g;
	u32 intr_mask;
	u32 intr_dest;

	nvgpu_flcn_set_irq(&g->sec2_flcn, false, 0x0, 0x0);

	if (enable) {
		/* dest 0=falcon, 1=host; level 0=irq0, 1=irq1 */
		intr_dest = psec_falcon_irqdest_host_gptmr_f(0)    |
			psec_falcon_irqdest_host_wdtmr_f(1)    |
			psec_falcon_irqdest_host_mthd_f(0)     |
			psec_falcon_irqdest_host_ctxsw_f(0)    |
			psec_falcon_irqdest_host_halt_f(1)     |
			psec_falcon_irqdest_host_exterr_f(0)   |
			psec_falcon_irqdest_host_swgen0_f(1)   |
			psec_falcon_irqdest_host_swgen1_f(0)   |
			psec_falcon_irqdest_host_ext_f(0xff)   |
			psec_falcon_irqdest_target_gptmr_f(1)  |
			psec_falcon_irqdest_target_wdtmr_f(0)  |
			psec_falcon_irqdest_target_mthd_f(0)   |
			psec_falcon_irqdest_target_ctxsw_f(0)  |
			psec_falcon_irqdest_target_halt_f(0)   |
			psec_falcon_irqdest_target_exterr_f(0) |
			psec_falcon_irqdest_target_swgen0_f(0) |
			psec_falcon_irqdest_target_swgen1_f(0) |
			psec_falcon_irqdest_target_ext_f(0xff);

		/* 0=disable, 1=enable */
		intr_mask = psec_falcon_irqmset_gptmr_f(1)  |
			psec_falcon_irqmset_wdtmr_f(1)  |
			psec_falcon_irqmset_mthd_f(0)   |
			psec_falcon_irqmset_ctxsw_f(0)  |
			psec_falcon_irqmset_halt_f(1)   |
			psec_falcon_irqmset_exterr_f(1) |
			psec_falcon_irqmset_swgen0_f(1) |
			psec_falcon_irqmset_swgen1_f(1);

		nvgpu_flcn_set_irq(&g->sec2_flcn, true, intr_mask, intr_dest);
	}
}

bool tu104_sec2_is_interrupted(struct nvgpu_sec2 *sec2)
{
	struct gk20a *g = sec2->g;
	u32 servicedpmuint = 0U;

	servicedpmuint = psec_falcon_irqstat_halt_true_f() |
			psec_falcon_irqstat_exterr_true_f() |
			psec_falcon_irqstat_swgen0_true_f();

	if (gk20a_readl(g, psec_falcon_irqstat_r()) &
		servicedpmuint) {
		return true;
	}

	return false;
}

void tu104_sec2_isr(struct gk20a *g)
{
	struct nvgpu_sec2 *sec2 = &g->sec2;
	struct nvgpu_falcon_queue *queue;
	u32 intr, mask;
	bool recheck = false;

	nvgpu_mutex_acquire(&sec2->isr_mutex);
	if (!sec2->isr_enabled) {
		nvgpu_mutex_release(&sec2->isr_mutex);
		return;
	}

	mask = gk20a_readl(g, psec_falcon_irqmask_r()) &
		gk20a_readl(g, psec_falcon_irqdest_r());

	intr = gk20a_readl(g, psec_falcon_irqstat_r());

	intr = gk20a_readl(g, psec_falcon_irqstat_r()) & mask;
	if (!intr) {
		gk20a_writel(g, psec_falcon_irqsclr_r(), intr);
		nvgpu_mutex_release(&sec2->isr_mutex);
		return;
	}

	if ((intr & psec_falcon_irqstat_halt_true_f()) != 0U) {
		nvgpu_err(g, "sec2 halt intr not implemented");
		nvgpu_flcn_dump_stats(&g->sec2_flcn);
	}
	if ((intr & psec_falcon_irqstat_exterr_true_f()) != 0U) {
		nvgpu_err(g,
			"sec2 exterr intr not implemented. Clearing interrupt.");

		gk20a_writel(g, psec_falcon_exterrstat_r(),
			gk20a_readl(g, psec_falcon_exterrstat_r()) &
				~psec_falcon_exterrstat_valid_m());
	}

	if ((intr & psec_falcon_irqstat_swgen0_true_f()) != 0U) {
		if (nvgpu_sec2_process_message(sec2)) {
			gk20a_writel(g, psec_falcon_irqsclr_r(), intr);
			goto exit;
		}
		recheck = true;
	}

	gk20a_writel(g, psec_falcon_irqsclr_r(), intr);

	if (recheck) {
		queue = &sec2->queue[SEC2_NV_MSGQ_LOG_ID];
		if (!nvgpu_flcn_queue_is_empty(sec2->flcn, queue)) {
			gk20a_writel(g, psec_falcon_irqsset_r(),
				psec_falcon_irqsset_swgen0_set_f());
		}
	}

exit:
	nvgpu_sec2_dbg(g, "Done");
	nvgpu_mutex_release(&sec2->isr_mutex);
}

void tu104_start_sec2_secure(struct gk20a *g)
{
	gk20a_writel(g, psec_falcon_cpuctl_alias_r(),
		psec_falcon_cpuctl_alias_startcpu_f(1U));
}
