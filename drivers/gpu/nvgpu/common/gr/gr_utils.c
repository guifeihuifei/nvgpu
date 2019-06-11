/*
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
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
#include <nvgpu/types.h>
#include <nvgpu/gr/gr_utils.h>

#include <nvgpu/gr/config.h>

#include "gr_priv.h"

struct nvgpu_gr_falcon *nvgpu_gr_get_falcon_ptr(struct gk20a *g)
{
	return g->gr->falcon;
}

void nvgpu_gr_reset_falcon_ptr(struct gk20a *g)
{
	g->gr->falcon = NULL;
}

struct nvgpu_gr_obj_ctx_golden_image *nvgpu_gr_get_golden_image_ptr(
							struct gk20a *g)
{
	return g->gr->golden_image;
}

void nvgpu_gr_reset_golden_image_ptr(struct gk20a *g)
{
	g->gr->golden_image = NULL;
}

#ifdef CONFIG_NVGPU_GRAPHICS
struct nvgpu_gr_zcull *nvgpu_gr_get_zcull_ptr(struct gk20a *g)
{
	return g->gr->zcull;
}

struct nvgpu_gr_zbc *nvgpu_gr_get_zbc_ptr(struct gk20a *g)
{
	return g->gr->zbc;
}
#endif

struct nvgpu_gr_config *nvgpu_gr_get_config_ptr(struct gk20a *g)
{
	return g->gr->config;
}

#ifdef CONFIG_NVGPU_DEBUGGER
struct nvgpu_gr_hwpm_map *nvgpu_gr_get_hwpm_map_ptr(struct gk20a *g)
{
	return g->gr->hwpm_map;
}
#endif

struct nvgpu_gr_intr *nvgpu_gr_get_intr_ptr(struct gk20a *g)
{
	return g->gr->intr;
}

#ifdef CONFIG_NVGPU_FECS_TRACE
struct nvgpu_gr_global_ctx_buffer_desc *nvgpu_gr_get_global_ctx_buffer_ptr(
							struct gk20a *g)
{
	return g->gr->global_ctx_buffer;
}
#endif

u32 nvgpu_gr_get_override_ecc_val(struct gk20a *g)
{
	return g->gr->fecs_feature_override_ecc_val;
}

void nvgpu_gr_override_ecc_val(struct gk20a *g, u32 ecc_val)
{
	g->gr->fecs_feature_override_ecc_val = ecc_val;
}

u32 nvgpu_gr_get_cilp_preempt_pending_chid(struct gk20a *g)
{
	return g->gr->cilp_preempt_pending_chid;
}

void nvgpu_gr_clear_cilp_preempt_pending_chid(struct gk20a *g)
{
	g->gr->cilp_preempt_pending_chid =
				NVGPU_INVALID_CHANNEL_ID;
}