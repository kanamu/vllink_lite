/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/
#include "../vsf_kernel_cfg.h"

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_USE_KERNEL == ENABLED

#define __VSF_THREAD_CLASS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT__
#include "../vsf_kernel_common.h"
#include "./vsf_thread.h"
#include "../vsf_os.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

SECTION("text.vsf.kernel.vsf_thread_get_cur")
vsf_thread_t *vsf_thread_get_cur(void)
{
    return (vsf_thread_t *)vsf_eda_get_cur();
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif


SECTION("text.vsf.kernel.vsf_thread_ret")
void vsf_thread_ret(void)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    class_internal(thread_obj, thread, vsf_thread_t);
    VSF_KERNEL_ASSERT(thread != NULL);
    
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
    VSF_KERNEL_ASSERT(NULL != this_ptr);
#   else
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
    VSF_KERNEL_ASSERT(NULL != this_ptr);
#   endif
    longjmp(*(this_ptr->ret), 0); 
#else
    longjmp(*(thread)->ret, 0);
#endif
}

SECTION("text.vsf.kernel.vsf_thread_exit")
void vsf_thread_exit(void)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    class_internal(thread_obj, thread, vsf_thread_t);
    VSF_KERNEL_ASSERT(thread != NULL);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
#   else
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
#   endif
    VSF_KERNEL_ASSERT(NULL != this_ptr);
    vsf_eda_return();
    longjmp(*(this_ptr->ret), 0);
#else
    vsf_eda_return();
    longjmp(*(thread)->ret, 0);
#endif
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION("text.vsf.kernel.vsf_thread_wait")
static vsf_evt_t __vsf_thread_wait(vsf_thread_cb_t *cb)
{
    vsf_evt_t curevt;
    jmp_buf pos;

    VSF_KERNEL_ASSERT(cb != NULL);
    class_internal(cb, this_ptr, vsf_thread_cb_t);

    this_ptr->pos = &pos;
    curevt = setjmp(*(this_ptr->pos));

    if (!curevt) {
        longjmp(*(this_ptr->ret), 0); 
    }
    return curevt;
}
#endif

SECTION("text.vsf.kernel.vsf_thread_wait")
vsf_evt_t vsf_thread_wait(void)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    class_internal(thread_obj, thread, vsf_thread_t);
    vsf_evt_t curevt;
    

    VSF_KERNEL_ASSERT(thread != NULL);
    
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame);
    curevt = __vsf_thread_wait((vsf_thread_cb_t *)thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param);
#   else
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_eda_t.fn.frame);
    curevt = __vsf_thread_wait((vsf_thread_cb_t *)thread->use_as__vsf_eda_t.fn.frame->ptr.param);
#   endif
#else
    jmp_buf pos;

    thread->pos = &pos;
    curevt = setjmp(*thread->pos);
    if (!curevt) {
        vsf_thread_ret();
    }
#endif
    
    return curevt;
}

SECTION("text.vsf.kernel.vsf_thread_wfe")

//! todo: check whether the new implementation stable and equivalent to the old implementation.
#if 0
void vsf_thread_wait_for_evt(vsf_evt_t evt)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    class_internal(thread_obj, thread, vsf_thread_t);
    vsf_evt_t curevt;
    jmp_buf pos;

    VSF_KERNEL_ASSERT(thread != NULL);
    
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
#   else
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
#   endif
    VSF_KERNEL_ASSERT(NULL != this_ptr);
    this_ptr->pos = &pos;
    curevt = setjmp(*(this_ptr->pos));
#else
    thread->pos = &pos;
    curevt = setjmp(*thread->pos);
    //pthread_obj->current_evt = curevt;
#endif

    if (!curevt || (curevt != evt)) {
        vsf_thread_ret();
    }
}
#else
void vsf_thread_wait_for_evt(vsf_evt_t evt)
{
    while(evt != vsf_thread_wait());
}
#endif

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION("text.vsf.kernel.vsf_thread_wait_for_evt_msg")
uintptr_t vsf_thread_wait_for_evt_msg(vsf_evt_t evt)
{
    vsf_thread_wfe(evt);
    return (uintptr_t)vsf_eda_get_cur_msg();
}

SECTION("text.vsf.kernel.vsf_thread_wait_for_evt_msg")
uintptr_t vsf_thread_wait_for_msg(void)
{
    return vsf_thread_wait_for_evt_msg(VSF_EVT_MESSAGE);
}
#else

SECTION("text.vsf.kernel.vsf_thread_wait_for_evt_msg")
uintptr_t vsf_thread_wait_for_msg(void)
{
    vsf_thread_wait();
    return (uintptr_t)vsf_eda_get_cur_msg();
}
#endif


SECTION("text.vsf.kernel.vsf_thread_sendevt")
void vsf_thread_sendevt(vsf_thread_t *thread, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(thread != NULL);
    class_internal(thread, this_ptr, vsf_thread_t);
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    vsf_eda_post_evt(&this_ptr->use_as__vsf_teda_t.use_as__vsf_eda_t, evt);
#else
    vsf_eda_post_evt(&this_ptr->use_as__vsf_eda_t, evt);
#endif
}

SECTION("text.vsf.kernel.vsf_thread")
static void __vsf_thread_entry(void)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    class_internal(thread_obj, thread, vsf_thread_t);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
    this_ptr->entry((vsf_thread_cb_t *)thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param);
#   else
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_eda_t.fn.frame);
    class_internal(thread->use_as__vsf_eda_t.fn.frame->ptr.param,
                    this_ptr, vsf_thread_cb_t);
    this_ptr->entry((vsf_thread_cb_t *)thread->use_as__vsf_eda_t.fn.frame->ptr.param);
#   endif
    VSF_KERNEL_ASSERT(NULL != this_ptr);
    vsf_eda_return();
    longjmp(*(this_ptr->ret), 0);
#else
    thread->entry(thread_obj);
    vsf_eda_return();
    longjmp(*(thread)->ret, 0);
#endif
}

SECTION("text.vsf.kernel.vsf_thread")
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
//static void __vsf_thread_evthandler(vsf_thread_cb_t *target, vsf_evt_t evt)
static void __vsf_thread_evthandler(uintptr_t local, vsf_evt_t evt)
{
    jmp_buf ret;

    VSF_KERNEL_ASSERT(local != (uintptr_t)NULL);
    class_internal( *(void **)((uintptr_t)local - sizeof(uintptr_t)), 
                    this_ptr, vsf_thread_cb_t);
    this_ptr->ret = &ret;
    if (!setjmp(ret)) {
        if (VSF_EVT_INIT == evt) {
            vsf_arch_set_stack((uintptr_t)(&this_ptr->stack[(this_ptr->stack_size>>3)]));
            __vsf_thread_entry();
            
        } else {
            longjmp(*(this_ptr->pos), evt);
        }
    } 
}
#else

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif

static void __vsf_thread_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    class_internal((vsf_thread_t *)eda, thread, vsf_thread_t);

    jmp_buf ret;

    VSF_KERNEL_ASSERT(thread != NULL);
    thread->ret = &ret;
    if (!setjmp(ret)) {
        if (VSF_EVT_INIT == evt) {
            vsf_arch_set_stack((uintptr_t)(&thread->stack[(thread->stack_size>>3)]));
            __vsf_thread_entry();
        } else {
            longjmp(*thread->pos, evt);
        }
    }
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#pragma GCC diagnostic pop
#endif

#endif

#if __IS_COMPILER_IAR__
//! bit mask appears to contain significant bits that do not affect the result 
#   pragma diag_suppress=pa182,pe111
#endif

SECTION("text.vsf.kernel.vsf_thread")
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
vsf_err_t vk_thread_start( vsf_thread_t *thread, 
                            vsf_thread_cb_t *thread_cb, 
                            vsf_prio_t priority)
{
    class_internal(thread, this_ptr, vsf_thread_t);
    VSF_KERNEL_ASSERT(this_ptr != NULL && NULL != thread_cb);
    VSF_KERNEL_ASSERT(      (0 != thread_cb->stack_size)
                        &&  (NULL != thread_cb->stack)
                        &&  (NULL != thread_cb->entry));
    
    vsf_eda_cfg_t cfg = {
        .fn.param_evthandler    = (vsf_param_eda_evthandler_t)__vsf_thread_evthandler,
        .priority               = priority,
        .target                 = (uintptr_t)thread_cb,
        .is_stack_owner         = true,
#   if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
#       if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        .on_terminate           = thread->use_as__vsf_teda_t.use_as__vsf_eda_t.on_terminate,
#       else
        .on_terminate           = thread->use_as__vsf_eda_t.on_terminate,
#       endif
#   endif
    };
    
    if (thread_cb->stack_size < (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
    } else if (0 != (thread_cb->stack_size & (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1))) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED;
    }

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    return vsf_teda_init_ex(&this_ptr->use_as__vsf_teda_t, &cfg);
#   else
    return vsf_eda_init_ex(&this_ptr->use_as__vsf_eda_t, &cfg);
#   endif
}

#if __IS_COMPILER_IAR__
//! bit mask appears to contain significant bits that do not affect the result 
#   pragma diag_warning=pa182,pe111
#endif

SECTION("text.vsf.kernel.vk_eda_call_thread_prepare")
vsf_err_t vk_eda_call_thread_prepare(vsf_thread_cb_t *thread_cb,
                                        vsf_thread_prepare_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(NULL != cfg && NULL != thread_cb);
    class_internal(thread_cb, this_ptr, vsf_thread_cb_t);
    VSF_KERNEL_ASSERT(    (NULL != cfg->entry)
                    &&    (NULL != cfg->stack)
                    &&    (cfg->stack_size >= 8));

    this_ptr->entry = cfg->entry;
    this_ptr->stack = cfg->stack;
    this_ptr->stack_size = cfg->stack_size;
    
    return VSF_ERR_NONE;
}

SECTION("text.vsf.kernel.vk_eda_call_thread")
vsf_err_t vk_eda_call_thread(vsf_thread_cb_t *thread_cb)
{
    VSF_KERNEL_ASSERT(NULL != thread_cb);
    vsf_err_t ret;
    __vsf_eda_frame_state_t state = { .bits = {.is_fsm = false,
                                      .is_stack_owner = true,}
                                    };
    ret = __vsf_eda_call_eda_ex(   (uintptr_t)__vsf_thread_evthandler, 
                                    (uintptr_t)thread_cb, 
                                    state, true);
    
    return ret;
}


SECTION("text.vsf.kernel.__vsf_thread_call_eda_ex")
static vsf_err_t __vsf_thread_call_eda_ex(  uintptr_t eda_handler, 
                                            uintptr_t param, 
                                            __vsf_eda_frame_state_t state,
                                            uintptr_t local_buff,
                                            size_t local_buff_size)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    class_internal(thread_obj, thread, vsf_thread_t);
    
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame);
    vsf_thread_cb_t *cb = (vsf_thread_cb_t *)thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param;
#   else
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_eda_t.fn.frame);
    vsf_thread_cb_t *cb = (vsf_thread_cb_t *)thread->use_as__vsf_eda_t.fn.frame->ptr.param;
#   endif
    VSF_KERNEL_ASSERT(NULL != cb);

    vsf_err_t err;
    vsf_evt_t evt;

    enum {
        VSF_THREAD_CALL_PEDA = 0,
        VSF_THREAD_WAIT_RETURN,
    };

    while (1) {
        err = __vsf_eda_call_eda_ex(eda_handler, param, state, true);
        if (VSF_ERR_NONE != err) {
            vsf_eda_yield();
        } else if ((uintptr_t)NULL != local_buff) {
            //! initialise local
            size_t size = min(state.local_size, local_buff_size);
            if (size > 0) {
                uintptr_t local = vsf_eda_get_local();
                memcpy((void *)local, (void *)local_buff, size);
            }
        }
        
        evt = __vsf_thread_wait(cb);

        if (evt == VSF_EVT_RETURN) {
            break;
        }
    }

    return err;
}


SECTION("text.vsf.kernel.vk_thread_call_eda")
vsf_err_t vk_thread_call_eda(   uintptr_t eda_handler, 
                                uintptr_t param, 
                                size_t local_size,
                                size_t local_buff_size,
                                uintptr_t local_buff)
{
    __vsf_eda_frame_state_t state = { .bits.is_fsm = false,.local_size = local_size};
    return __vsf_thread_call_eda_ex(eda_handler, param, state, local_buff, local_buff_size);
}

SECTION("text.vsf.kernel.vsf_thread_call_thread")
vsf_err_t vk_thread_call_thread(vsf_thread_cb_t *thread_cb,
                                vsf_thread_prepare_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(NULL != cfg && NULL != thread_cb);
    class_internal(thread_cb, this_ptr, vsf_thread_cb_t);
    
    __vsf_eda_frame_state_t state = { .bits = {.is_fsm = false,
                                      .is_stack_owner = true,
                                      },
                                    };
    VSF_KERNEL_ASSERT(    (NULL != cfg->entry)
                    &&    (NULL != cfg->stack)
                    &&    (cfg->stack_size >= 8));
    
    this_ptr->entry = cfg->entry;
    this_ptr->stack = cfg->stack;
    this_ptr->stack_size = cfg->stack_size;
    
    return __vsf_thread_call_eda_ex((uintptr_t)__vsf_thread_evthandler, 
                                    (uintptr_t)thread_cb, state, (uintptr_t)NULL, 0);
}

SECTION("text.vsf.kernel.vk_thread_call_fsm")
fsm_rt_t vk_thread_call_fsm(vsf_fsm_entry_t eda_handler, uintptr_t param, size_t local_size)
{
    vsf_thread_t *thread_obj = vsf_thread_get_cur();
    class_internal(thread_obj, thread, vsf_thread_t);
    
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame);
    vsf_thread_cb_t *cb = (vsf_thread_cb_t *)thread->use_as__vsf_teda_t.use_as__vsf_eda_t.fn.frame->ptr.param;
#   else
    VSF_KERNEL_ASSERT(NULL != thread->use_as__vsf_eda_t.fn.frame);
    vsf_thread_cb_t *cb = (vsf_thread_cb_t *)thread->use_as__vsf_eda_t.fn.frame->ptr.param;
#   endif
    VSF_KERNEL_ASSERT(NULL != cb);
    
    fsm_rt_t ret;

    while (1) {
        ret = __vsf_eda_call_fsm(eda_handler, param, local_size);
        if (fsm_rt_on_going == ret) {
            vsf_eda_yield();
        } else {
            break;
        }
        __vsf_thread_wait(cb);
    }
    
    return ret;
}
#else
vsf_err_t vk_thread_start(vsf_thread_t *thread, vsf_prio_t priority)
{
    class_internal(thread, this_ptr, vsf_thread_t)
    VSF_KERNEL_ASSERT(this_ptr != NULL);
    VSF_KERNEL_ASSERT(      (NULL != this_ptr->entry)
                        &&  (NULL != this_ptr->stack)
                        &&  (0 != this_ptr->stack_size));
    this_ptr->fn.evthandler = __vsf_thread_evthandler;

    if (this_ptr->stack_size < (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
    } else if (0 != (this_ptr->stack_size & (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1))) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED;
    }

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    return vsf_teda_init(&this_ptr->use_as__vsf_teda_t, priority, true);
#   else
    return vsf_eda_init(&this_ptr->use_as__vsf_eda_t, priority, true);
#   endif
}
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION("text.vsf.kernel.vsf_thread_delay")
void vsf_thread_delay(uint_fast32_t tick)
{
    vsf_teda_set_timer(tick);
    vsf_thread_wfe(VSF_EVT_TIMER);
}
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED

SECTION("text.vsf.kernel.__vsf_thread_wait_for_sync")
static vsf_sync_reason_t __vsf_thread_wait_for_sync(vsf_sync_t *sync, int_fast32_t time_out)
{
    vsf_err_t err;
    vsf_sync_reason_t reason;

    err = vsf_eda_sync_decrease(sync, time_out);
    if (!err) { return VSF_SYNC_GET; }
    else if (err < 0) { return VSF_SYNC_FAIL; }
    //else {
        do {
            reason = vsf_eda_sync_get_reason(sync, vsf_thread_wait());
        } while (reason == VSF_SYNC_PENDING);
        return reason;
    //}
}

SECTION("text.vsf.kernel.__vsf_thread_wait_for_sync")
vsf_sync_reason_t vsf_thread_sem_pend(vsf_sem_t* sem, int_fast32_t timeout)
{
    return __vsf_thread_wait_for_sync(sem, timeout);
}

SECTION("text.vsf.kernel.__vsf_thread_wait_for_sync")
vsf_sync_reason_t vsf_thread_trig_pend(vsf_trig_t* trig, int_fast32_t timeout)
{
    return __vsf_thread_wait_for_sync(trig, timeout);
}

SECTION("text.vsf.kernel.vsf_thread_mutex")
vsf_sync_reason_t vsf_thread_mutex_enter(vsf_mutex_t *mtx, int_fast32_t timeout)
{
    return __vsf_thread_wait_for_sync(&mtx->use_as__vsf_sync_t, timeout);
}

SECTION("text.vsf.kernel.vsf_thread_mutex")
vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx)
{
    return vsf_eda_mutex_leave(mtx);
}

SECTION("text.vsf.kernel.vsf_thread_sem_post")
vsf_err_t vsf_thread_sem_post(vsf_sem_t *sem)
{
    return vsf_eda_sem_post(sem);
}

#if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED

SECTION("text.vsf.kernel.vsf_thread_bmpevt_pend")
vsf_sync_reason_t vsf_thread_bmpevt_pend(
                    vsf_bmpevt_t *bmpevt,
                    vsf_bmpevt_pender_t *pender,
                    int_fast32_t timeout)
{
    vsf_sync_reason_t reason;
    vsf_err_t err;
    vsf_evt_t evt;

    err = vsf_eda_bmpevt_pend(bmpevt, pender, timeout);
    if (!err) { return VSF_SYNC_GET; }
    else if (err < 0) { return VSF_SYNC_FAIL; }
    else {
        while (1) {
            evt = vsf_thread_wait();
            reason = vsf_eda_bmpevt_poll(bmpevt, pender, evt);
            if (reason != VSF_SYNC_PENDING) {
                return reason;
            }
        }
    }
}
#endif
#endif      // VSF_KERNEL_CFG_SUPPORT_SYNC

#endif
