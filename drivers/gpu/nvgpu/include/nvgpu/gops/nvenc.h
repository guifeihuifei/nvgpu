/*
 * Copyright (c) 2023, NVIDIA CORPORATION.  All rights reserved.
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
#ifndef NVGPU_GOPS_NVENC_H
#define NVGPU_GOPS_NVENC_H

struct nvgpu_multimedia_ctx;

struct gops_nvenc {

	u32 (*base_addr)(void);
	/**
	 * Initializes nvenc unit private data struct in the GPU driver based on
	 * the current chip.
	 */
	int (*init)(struct gk20a *g);

	/**
	 * @brief Deinitialize NVENC support.
	 *
	 * @param g [in]		Pointer to GPU driver struct.
	 *
	 * This function de-allocates memory allocated during initialization.
	 */
	int (*deinit)(struct gk20a *g);

	/**
	 * @brief Reset NVENC.
	 *
	 * @param g [in]		Pointer to GPU driver struct.
	 *
	 * This function resets the NVENC HW unit and loads the FW again.
	 */
	int (*reset)(struct gk20a *g);

	/**
	 * Load and bootstrap ucode on nvenc falcon.
	 */
	int (*bootstrap)(struct gk20a *g);
	void (*setup_boot_config)(struct gk20a *g);
	void (*halt_engine)(struct gk20a *g);
	void (*set_irq_regs)(struct gk20a *g, struct nvgpu_falcon *flcn);
	void (*interface_enable)(struct gk20a *g);

	/**
	 * @brief Allocate and setup engine context for GPU channel.
	 *
	 * @param ch [in]		Pointer to GPU channel.
	 * @param class_num [in]	GPU class ID.
	 * @param flags [in]		Flags for context allocation.
	 *
	 * This HAL allocates and sets up engine context for
	 * a GPU channel.
	 *
	 * @return 0 in case of success, < 0 in case of failure.
	 * @retval -ENOMEM if memory allocation for context buffer fails.
	 * @retval -EINVAL if invalid GPU class ID is provided.
	 */
	int (*multimedia_alloc_ctx)(struct nvgpu_channel *ch,
			     u32 class_num, u32 flags);

	/**
	 * @brief Free engine context buffer.
	 *
	 * @param g [in]	Pointer to GPU driver struct.
	 * @param eng_ctx [in]	Pointer to engine context data.
	 *
	 * This function will free the memory allocated for engine
	 * context buffer.
	 */
	void (*multimedia_free_ctx)(struct gk20a *g,
			    struct nvgpu_multimedia_ctx *eng_ctx);
};

#endif /* NVGPU_GOPS_NVENC_H */