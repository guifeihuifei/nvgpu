/*
 * Copyright (c) 2018-2020, NVIDIA CORPORATION.  All rights reserved.
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
#ifndef NVGPU_SYNC_SYNCPT_CMDBUF_GV11B_H
#define NVGPU_SYNC_SYNCPT_CMDBUF_GV11B_H

#include <nvgpu/types.h>
#include <nvgpu/errno.h>

struct gk20a;
struct priv_cmd_entry;
struct nvgpu_mem;
struct nvgpu_channel;
struct vm_gk20a;

#ifdef CONFIG_TEGRA_GK20A_NVHOST

#ifdef CONFIG_NVGPU_KERNEL_MODE_SUBMIT
void gv11b_syncpt_add_wait_cmd(struct gk20a *g,
		struct priv_cmd_entry *cmd,
		u32 id, u32 thresh, u64 gpu_va_base);
u32 gv11b_syncpt_get_wait_cmd_size(void);
u32 gv11b_syncpt_get_incr_per_release(void);
void gv11b_syncpt_add_incr_cmd(struct gk20a *g,
		struct priv_cmd_entry *cmd,
		u32 id, u64 gpu_va, bool wfi);
u32 gv11b_syncpt_get_incr_cmd_size(bool wfi_cmd);
#endif /* CONFIG_NVGPU_KERNEL_MODE_SUBMIT */

void gv11b_syncpt_free_buf(struct nvgpu_channel *c,
		struct nvgpu_mem *syncpt_buf);

int gv11b_syncpt_alloc_buf(struct nvgpu_channel *c,
		u32 syncpt_id, struct nvgpu_mem *syncpt_buf);

int gv11b_syncpt_get_sync_ro_map(struct vm_gk20a *vm,
		u64 *base_gpuva, u32 *sync_size);

#else

static inline void gv11b_syncpt_free_buf(struct nvgpu_channel *c,
		struct nvgpu_mem *syncpt_buf)
{
}

static inline int gv11b_syncpt_alloc_buf(struct nvgpu_channel *c,
		u32 syncpt_id, struct nvgpu_mem *syncpt_buf)
{
	return -EINVAL;
}

static inline int gv11b_syncpt_get_sync_ro_map(struct vm_gk20a *vm,
		u64 *base_gpuva, u32 *sync_size)
{
	return -EINVAL;
}

#endif /* CONFIG_TEGRA_GK20A_NVHOST */

#endif /* NVGPU_SYNC_SYNCPT_CMDBUF_GV11B_H */
