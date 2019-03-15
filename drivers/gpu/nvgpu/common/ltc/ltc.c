/*
 * Copyright (c) 2017-2019, NVIDIA CORPORATION.  All rights reserved.
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

#include <nvgpu/ltc.h>
#include <nvgpu/dma.h>
#include <nvgpu/nvgpu_mem.h>
#include <nvgpu/gk20a.h>

#include "gk20a/gr_gk20a.h"

void nvgpu_ltc_remove_support(struct gk20a *g)
{
	struct nvgpu_ltc *ltc = g->ltc;

	nvgpu_log_fn(g, " ");

	if (ltc == NULL) {
		return;
	}

	nvgpu_kfree(g, ltc);
	g->ltc = NULL;
}


int nvgpu_init_ltc_support(struct gk20a *g)
{
	struct nvgpu_ltc *ltc = g->ltc;

	nvgpu_log_fn(g, " ");


	g->mm.ltc_enabled_current = true;
	g->mm.ltc_enabled_target = true;

	if (ltc == NULL) {
		ltc = nvgpu_kzalloc(g, sizeof(*ltc));
		if (ltc == NULL) {
			return -ENOMEM;
		}
		g->ltc = ltc;
		nvgpu_spinlock_init(&g->ltc->ltc_enabled_lock);
	}

	if (g->ops.ltc.init_fs_state != NULL) {
		g->ops.ltc.init_fs_state(g);
	}

	return 0;
}

void nvgpu_ltc_sync_enabled(struct gk20a *g)
{
	if (g->ops.ltc.set_enabled == NULL) {
		return;
	}

	nvgpu_spinlock_acquire(&g->ltc->ltc_enabled_lock);
	if (g->mm.ltc_enabled_current != g->mm.ltc_enabled_target) {
		g->ops.ltc.set_enabled(g, g->mm.ltc_enabled_target);
		g->mm.ltc_enabled_current = g->mm.ltc_enabled_target;
	}
	nvgpu_spinlock_release(&g->ltc->ltc_enabled_lock);
}

u32 nvgpu_ltc_get_ltc_count(struct gk20a *g)
{
	return g->ltc->ltc_count;
}

u32 nvgpu_ltc_get_slices_per_ltc(struct gk20a *g)
{
	return g->ltc->slices_per_ltc;
}

u32 nvgpu_ltc_get_cacheline_size(struct gk20a *g)
{
	return g->ltc->cacheline_size;
}
