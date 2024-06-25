/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"
#include "rt_test_root.h"

/**
 * @file    rt_test_sequence_005.c
 * @brief   Test Sequence 005 code.
 *
 * @page rt_test_sequence_005 [5] Threads Functionality
 *
 * File: @ref rt_test_sequence_005.c
 *
 * <h2>Description</h2>
 * This sequence tests the ChibiOS/RT functionalities related to
 * threading.
 *
 * <h2>Test Cases</h2>
 * - @subpage rt_test_005_001
 * - @subpage rt_test_005_002
 * - @subpage rt_test_005_003
 * - @subpage rt_test_005_004
 * - @subpage rt_test_005_005
 * .
 */

/****************************************************************************
 * Shared code.
 ****************************************************************************/

static THD_FUNCTION(thread, p) {

  test_emit_token(*(char *)p);
}

#if (CH_CFG_USE_THREADHIERARCHY)

#define CHILDTHREAD_SLEEP_MS          10

static THD_FUNCTION(hierarchythread, p) {

  do {
    if (*(tprio_t *)p != chThdGetPriorityX()) {
      chThdSetPriority(*(tprio_t *)p);
    }
    chThdSleepMilliseconds(CHILDTHREAD_SLEEP_MS);
  } while (!chThdShouldTerminateX());
}

#endif

/****************************************************************************
 * Test cases.
 ****************************************************************************/

/**
 * @page rt_test_005_001 [5.1] Thread Sleep functionality
 *
 * <h2>Description</h2>
 * The functionality of @p chThdSleep() and derivatives is tested.
 *
 * <h2>Test Steps</h2>
 * - [5.1.1] The current system time is read then a sleep is performed
 *   for 100 system ticks and on exit the system time is verified
 *   again.
 * - [5.1.2] The current system time is read then a sleep is performed
 *   for 100000 microseconds and on exit the system time is verified
 *   again.
 * - [5.1.3] The current system time is read then a sleep is performed
 *   for 100 milliseconds and on exit the system time is verified
 *   again.
 * - [5.1.4] The current system time is read then a sleep is performed
 *   for 1 second and on exit the system time is verified again.
 * - [5.1.5] Function chThdSleepUntil() is tested with a timeline of
 *   "now" + 100 ticks.
 * .
 */

static void rt_test_005_001_execute(void) {
  systime_t time;

  /* [5.1.1] The current system time is read then a sleep is performed
     for 100 system ticks and on exit the system time is verified
     again.*/
  test_set_step(1);
  {
    time = chVTGetSystemTimeX();
    chThdSleep(100);
    test_assert_time_window(chTimeAddX(time, 100),
                            chTimeAddX(time, 100 + CH_CFG_ST_TIMEDELTA + 1),
                            "out of time window");
  }
  test_end_step(1);

  /* [5.1.2] The current system time is read then a sleep is performed
     for 100000 microseconds and on exit the system time is verified
     again.*/
  test_set_step(2);
  {
    time_usecs_t us = TIME_I2US(test_limit_interval(TIME_US2I(100000), CH_CFG_ST_TIMEDELTA + 1));
    if (us > 1) {
      us -= 1;
      time = chVTGetSystemTimeX();
      chThdSleepMicroseconds(us);
      test_assert_time_window(chTimeAddX(time, TIME_US2I(us)),
                              chTimeAddX(time, TIME_US2I(us) + CH_CFG_ST_TIMEDELTA + 1),
                              "out of time window");
    }
  }
  test_end_step(2);

  /* [5.1.3] The current system time is read then a sleep is performed
     for 100 milliseconds and on exit the system time is verified
     again.*/
  test_set_step(3);
  {
    time_msecs_t ms = TIME_I2MS(test_limit_interval(TIME_MS2I(100), CH_CFG_ST_TIMEDELTA + 1));
    if (ms > 1) {
      ms -= 1;
      time = chVTGetSystemTimeX();
      chThdSleepMilliseconds(ms);
      test_assert_time_window(chTimeAddX(time, TIME_MS2I(ms)),
                              chTimeAddX(time, TIME_MS2I(ms) + CH_CFG_ST_TIMEDELTA + 1),
                              "out of time window");
    }
  }
  test_end_step(3);

  /* [5.1.4] The current system time is read then a sleep is performed
     for 1 second and on exit the system time is verified again.*/
  test_set_step(4);
  {
    time_secs_t s = TIME_I2S(test_limit_interval(TIME_S2I(1), CH_CFG_ST_TIMEDELTA + 1));
    if (s > 1) {
      s -= 1;
      time = chVTGetSystemTimeX();
      chThdSleepSeconds(s);
      test_assert_time_window(chTimeAddX(time, TIME_S2I(s)),
                              chTimeAddX(time, TIME_S2I(s) + CH_CFG_ST_TIMEDELTA + 1),
                              "out of time window");
    }
  }
  test_end_step(4);

  /* [5.1.5] Function chThdSleepUntil() is tested with a timeline of
     "now" + 100 ticks.*/
  test_set_step(5);
  {
    time = chVTGetSystemTimeX();
    chThdSleepUntil(chTimeAddX(time, 100));
    test_assert_time_window(chTimeAddX(time, 100),
                            chTimeAddX(time, 100 + CH_CFG_ST_TIMEDELTA + 1),
                            "out of time window");
  }
  test_end_step(5);
}

static const testcase_t rt_test_005_001 = {
  "Thread Sleep functionality",
  NULL,
  NULL,
  rt_test_005_001_execute
};

/**
 * @page rt_test_005_002 [5.2] Ready List functionality, threads priority order
 *
 * <h2>Description</h2>
 * Five threads, are enqueued in the ready list and atomically
 * executed. The test expects the threads to perform their operations
 * in correct priority order regardless of the initial order.
 *
 * <h2>Test Steps</h2>
 * - [5.2.1] Creating 5 threads with increasing priority, execution
 *   sequence is tested.
 * - [5.2.2] Creating 5 threads with decreasing priority, execution
 *   sequence is tested.
 * - [5.2.3] Creating 5 threads with pseudo-random priority, execution
 *   sequence is tested.
 * .
 */

static void rt_test_005_002_execute(void) {

  /* [5.2.1] Creating 5 threads with increasing priority, execution
     sequence is tested.*/
  test_set_step(1);
  {
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriorityX()-5, thread, "E");
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, chThdGetPriorityX()-4, thread, "D");
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, chThdGetPriorityX()-3, thread, "C");
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, chThdGetPriorityX()-2, thread, "B");
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, chThdGetPriorityX()-1, thread, "A");
    test_wait_threads();
    test_assert_sequence("ABCDE", "invalid sequence");
  }
  test_end_step(1);

  /* [5.2.2] Creating 5 threads with decreasing priority, execution
     sequence is tested.*/
  test_set_step(2);
  {
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, chThdGetPriorityX()-1, thread, "A");
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, chThdGetPriorityX()-2, thread, "B");
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, chThdGetPriorityX()-3, thread, "C");
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, chThdGetPriorityX()-4, thread, "D");
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriorityX()-5, thread, "E");
    test_wait_threads();
    test_assert_sequence("ABCDE", "invalid sequence");
  }
  test_end_step(2);

  /* [5.2.3] Creating 5 threads with pseudo-random priority, execution
     sequence is tested.*/
  test_set_step(3);
  {
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, chThdGetPriorityX()-4, thread, "D");
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriorityX()-5, thread, "E");
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, chThdGetPriorityX()-1, thread, "A");
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, chThdGetPriorityX()-2, thread, "B");
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, chThdGetPriorityX()-3, thread, "C");
    test_wait_threads();
    test_assert_sequence("ABCDE", "invalid sequence");
  }
  test_end_step(3);
}

static const testcase_t rt_test_005_002 = {
  "Ready List functionality, threads priority order",
  NULL,
  NULL,
  rt_test_005_002_execute
};

/**
 * @page rt_test_005_003 [5.3] Priority change test
 *
 * <h2>Description</h2>
 * A series of priority changes are performed on the current thread in
 * order to verify that the priority change happens as expected.
 *
 * <h2>Test Steps</h2>
 * - [5.3.1] Thread priority is increased by one then a check is
 *   performed.
 * - [5.3.2] Thread priority is returned to the previous value then a
 *   check is performed.
 * .
 */

static void rt_test_005_003_execute(void) {
  tprio_t prio, p1;

  /* [5.3.1] Thread priority is increased by one then a check is
     performed.*/
  test_set_step(1);
  {
    prio = chThdGetPriorityX();
    p1 = chThdSetPriority(prio + 1);
    test_assert(p1 == prio, "unexpected returned priority level");
    test_assert(chThdGetPriorityX() == prio + 1, "unexpected priority level");
  }
  test_end_step(1);

  /* [5.3.2] Thread priority is returned to the previous value then a
     check is performed.*/
  test_set_step(2);
  {
    p1 = chThdSetPriority(p1);
    test_assert(p1 == prio + 1, "unexpected returned priority level");
    test_assert(chThdGetPriorityX() == prio, "unexpected priority level");
  }
  test_end_step(2);
}

static const testcase_t rt_test_005_003 = {
  "Priority change test",
  NULL,
  NULL,
  rt_test_005_003_execute
};

#if (CH_CFG_USE_MUTEXES == TRUE) || defined(__DOXYGEN__)
/**
 * @page rt_test_005_004 [5.4] Priority change test with Priority Inheritance
 *
 * <h2>Description</h2>
 * A series of priority changes are performed on the current thread in
 * order to verify that the priority change happens as expected.
 *
 * <h2>Conditions</h2>
 * This test is only executed if the following preprocessor condition
 * evaluates to true:
 * - CH_CFG_USE_MUTEXES == TRUE
 * .
 *
 * <h2>Test Steps</h2>
 * - [5.4.1] Simulating a priority boost situation (prio > realprio).
 * - [5.4.2] Raising thread priority above original priority but below
 *   the boosted level.
 * - [5.4.3] Raising thread priority above the boosted level.
 * - [5.4.4] Restoring original conditions.
 * .
 */

static void rt_test_005_004_execute(void) {
  tprio_t prio, p1;

  /* [5.4.1] Simulating a priority boost situation (prio > realprio).*/
  test_set_step(1);
  {
    prio = chThdGetPriorityX();
    chThdGetSelfX()->hdr.pqueue.prio += 2;
    test_assert(chThdGetPriorityX() == prio + 2, "unexpected priority level");
  }
  test_end_step(1);

  /* [5.4.2] Raising thread priority above original priority but below
     the boosted level.*/
  test_set_step(2);
  {
    p1 = chThdSetPriority(prio + 1);
    test_assert(p1 == prio, "unexpected returned priority level");
    test_assert(chThdGetSelfX()->hdr.pqueue.prio == prio + 2, "unexpected priority level");
    test_assert(chThdGetSelfX()->realprio == prio + 1, "unexpected returned real priority level");
  }
  test_end_step(2);

  /* [5.4.3] Raising thread priority above the boosted level.*/
  test_set_step(3);
  {
    p1 = chThdSetPriority(prio + 3);
    test_assert(p1 == prio + 1, "unexpected returned priority level");
    test_assert(chThdGetSelfX()->hdr.pqueue.prio == prio + 3, "unexpected priority level");
    test_assert(chThdGetSelfX()->realprio == prio + 3, "unexpected real priority level");
  }
  test_end_step(3);

  /* [5.4.4] Restoring original conditions.*/
  test_set_step(4);
  {
    chSysLock();
    chThdGetSelfX()->hdr.pqueue.prio = prio;
    chThdGetSelfX()->realprio = prio;
    chSysUnlock();
  }
  test_end_step(4);
}

static const testcase_t rt_test_005_004 = {
  "Priority change test with Priority Inheritance",
  NULL,
  NULL,
  rt_test_005_004_execute
};
#endif /* CH_CFG_USE_MUTEXES == TRUE */

#if (CH_CFG_USE_THREADHIERARCHY) || defined(__DOXYGEN__)
/**
 * @page rt_test_005_005 [5.5] Thread hierarach with (un)ordered lists.
 *
 * <h2>Description</h2>
 * The functionality of the thread hierarchy is tested.
 *
 * <h2>Conditions</h2>
 * This test is only executed if the following preprocessor condition
 * evaluates to true:
 * - CH_CFG_USE_THREADHIERARCHY
 * This test comprises additional tests if the following preprocessor
 * condition evaluates to true:
 * - CH_CFG_THREADHIERARCHY_ORDERED
 * .
 *
 * <h2>Test Steps</h2>
 * - [5.5.1] Creating 5 child threads with increasing priority, hierarchy
 *           information is tested.
 * - [5.5.2] Creating 5 child threads with decreasing priority, hierarchy
 *           information is tested.
 * - [5.5.3] Creating 5 child threads with identical priority, hierarchy
 *           information is tested.
 * - [5.5.4] Creating 5 child threads with increasing priority which are
 *           terminated in a controlled manner, hierarchy information is tested.
 * - [5.5.5] Creating 5 child threads with increasing priority and then
 *           increasing the priority of a low-priority child, hierarchy
 *           information is tested.
 * - [5.5.6] Creating 5 child threads with increasing priority and then
 *           decreasing the priority of a high-priority child, hierarchy
 *           information is tested.
 */

static void rt_test_005_005_execute(void) {

  /* [5.5.1] Creating 5 child threads with increasing priority, hierarchy
   * information is tested. */
  test_set_step(1);
  {
    tprio_t prios[MAX_THREADS];
    thread_t *threads_cpy[MAX_THREADS];
    thread_t *parents[MAX_THREADS];
    thread_t *siblings[MAX_THREADS];
    prios[0] = chThdGetPriorityX()-5;
    prios[1] = chThdGetPriorityX()-4;
    prios[2] = chThdGetPriorityX()-3;
    prios[3] = chThdGetPriorityX()-2;
    prios[4] = chThdGetPriorityX()-1;
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, prios[0], hierarchythread, &prios[0]);
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, prios[1], hierarchythread, &prios[1]);
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, prios[2], hierarchythread, &prios[2]);
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, prios[3], hierarchythread, &prios[3]);
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, prios[4], hierarchythread, &prios[4]);
    threads_cpy[0] = threads[0];
    threads_cpy[1] = threads[1];
    threads_cpy[2] = threads[2];
    threads_cpy[3] = threads[3];
    threads_cpy[4] = threads[4];
    parents[0] = threads[0]->parent;
    parents[1] = threads[1]->parent;
    parents[2] = threads[2]->parent;
    parents[3] = threads[3]->parent;
    parents[4] = threads[4]->parent;
    siblings[0] = threads[0]->sibling;
    siblings[1] = threads[1]->sibling;
    siblings[2] = threads[2]->sibling;
    siblings[3] = threads[3]->sibling;
    siblings[4] = threads[4]->sibling;
    test_terminate_threads();
    test_wait_threads();
    test_assert(parents[0] == chThdGetSelfX() &&
                parents[1] == chThdGetSelfX() &&
                parents[2] == chThdGetSelfX() &&
                parents[3] == chThdGetSelfX() &&
                parents[4] == chThdGetSelfX() &&
                siblings[1] == threads_cpy[0] &&
                siblings[2] == threads_cpy[1] &&
                siblings[3] == threads_cpy[2] &&
                siblings[4] == threads_cpy[3], "invalid children list");
  }
  test_end_step(1);

  /* [5.5.2] Creating 5 child threads with decreasing priority, hierarchy
   * information is tested. */
  test_set_step(2);
  {
    tprio_t prios[MAX_THREADS];
    thread_t *threads_cpy[MAX_THREADS];
    thread_t *parents[MAX_THREADS];
    thread_t *siblings[MAX_THREADS];
    prios[0] = chThdGetPriorityX()-1;
    prios[1] = chThdGetPriorityX()-2;
    prios[2] = chThdGetPriorityX()-3;
    prios[3] = chThdGetPriorityX()-4;
    prios[4] = chThdGetPriorityX()-5;
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, prios[0], hierarchythread, &prios[0]);
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, prios[1], hierarchythread, &prios[1]);
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, prios[2], hierarchythread, &prios[2]);
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, prios[3], hierarchythread, &prios[3]);
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, prios[4], hierarchythread, &prios[4]);
    threads_cpy[0] = threads[0];
    threads_cpy[1] = threads[1];
    threads_cpy[2] = threads[2];
    threads_cpy[3] = threads[3];
    threads_cpy[4] = threads[4];
    parents[0] = threads[0]->parent;
    parents[1] = threads[1]->parent;
    parents[2] = threads[2]->parent;
    parents[3] = threads[3]->parent;
    parents[4] = threads[4]->parent;
    siblings[0] = threads[0]->sibling;
    siblings[1] = threads[1]->sibling;
    siblings[2] = threads[2]->sibling;
    siblings[3] = threads[3]->sibling;
    siblings[4] = threads[4]->sibling;
    test_terminate_threads();
    test_wait_threads();
#if CH_CFG_THREADHIERARCHY_ORDERED == TRUE
    test_assert(parents[0] == chThdGetSelfX() &&
                parents[1] == chThdGetSelfX() &&
                parents[2] == chThdGetSelfX() &&
                parents[3] == chThdGetSelfX() &&
                parents[4] == chThdGetSelfX() &&
                siblings[0] == threads_cpy[1] &&
                siblings[1] == threads_cpy[2] &&
                siblings[2] == threads_cpy[3] &&
                siblings[3] == threads_cpy[4], "invalid children list");
#else
    test_assert(parents[0] == chThdGetSelfX() &&
                parents[1] == chThdGetSelfX() &&
                parents[2] == chThdGetSelfX() &&
                parents[3] == chThdGetSelfX() &&
                parents[4] == chThdGetSelfX() &&
                siblings[1] == threads_cpy[0] &&
                siblings[2] == threads_cpy[1] &&
                siblings[3] == threads_cpy[2] &&
                siblings[4] == threads_cpy[3], "invalid children list");
#endif
  }
  test_end_step(2);

  /* [5.5.3] Creating 5 child threads with identical priority, hierarchy
   * information is tested. */
  test_set_step(3);
  {
    tprio_t prios[MAX_THREADS];
    thread_t *threads_cpy[MAX_THREADS];
    thread_t *parents[MAX_THREADS];
    thread_t *siblings[MAX_THREADS];
    prios[0] = chThdGetPriorityX()-1;
    prios[1] = chThdGetPriorityX()-1;
    prios[2] = chThdGetPriorityX()-1;
    prios[3] = chThdGetPriorityX()-1;
    prios[4] = chThdGetPriorityX()-1;
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, prios[0], hierarchythread, &prios[0]);
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, prios[1], hierarchythread, &prios[1]);
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, prios[2], hierarchythread, &prios[2]);
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, prios[3], hierarchythread, &prios[3]);
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, prios[4], hierarchythread, &prios[4]);
    threads_cpy[0] = threads[0];
    threads_cpy[1] = threads[1];
    threads_cpy[2] = threads[2];
    threads_cpy[3] = threads[3];
    threads_cpy[4] = threads[4];
    parents[0] = threads[0]->parent;
    parents[1] = threads[1]->parent;
    parents[2] = threads[2]->parent;
    parents[3] = threads[3]->parent;
    parents[4] = threads[4]->parent;
    siblings[0] = threads[0]->sibling;
    siblings[1] = threads[1]->sibling;
    siblings[2] = threads[2]->sibling;
    siblings[3] = threads[3]->sibling;
    siblings[4] = threads[4]->sibling;
    test_terminate_threads();
    test_wait_threads();
    test_assert(parents[0] == chThdGetSelfX() &&
                parents[1] == chThdGetSelfX() &&
                parents[2] == chThdGetSelfX() &&
                parents[3] == chThdGetSelfX() &&
                parents[4] == chThdGetSelfX() &&
                siblings[1] == threads_cpy[0] &&
                siblings[2] == threads_cpy[1] &&
                siblings[3] == threads_cpy[2] &&
                siblings[4] == threads_cpy[3] , "invalid children list");
  }
  test_end_step(3);

  /* [5.5.4] Creating 5 child threads with increasing priority, which are
   * terminated in a controlled manner, hierarchy information is tested. */
  test_set_step(4);
  {
    tprio_t prios[MAX_THREADS];
    thread_t *threads_cpy[MAX_THREADS];
    thread_t *parents[MAX_THREADS];
    thread_t *siblings[MAX_THREADS];
    prios[0] = chThdGetPriorityX()-5;
    prios[1] = chThdGetPriorityX()-4;
    prios[2] = chThdGetPriorityX()-3;
    prios[3] = chThdGetPriorityX()-2;
    prios[4] = chThdGetPriorityX()-1;
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, prios[0], hierarchythread, &prios[0]);
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, prios[1], hierarchythread, &prios[1]);
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, prios[2], hierarchythread, &prios[2]);
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, prios[3], hierarchythread, &prios[3]);
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, prios[4], hierarchythread, &prios[4]);
    threads_cpy[0] = threads[0];
    threads_cpy[1] = threads[1];
    threads_cpy[2] = threads[2];
    threads_cpy[3] = threads[3];
    threads_cpy[4] = threads[4];
#if CH_CFG_USE_REGISTRY == TRUE
    chThdAddRef(threads[1]);
    chThdAddRef(threads[4]);
#endif
    chThdTerminate(threads[1]);
    chThdTerminate(threads[4]);
    chThdWait(threads[1]);
    chThdWait(threads[4]);
    parents[0] = threads[0]->parent;
    parents[1] = threads[1]->parent;
    parents[2] = threads[2]->parent;
    parents[3] = threads[3]->parent;
    parents[4] = threads[4]->parent;
    siblings[0] = threads[0]->sibling;
    siblings[1] = threads[1]->sibling;
    siblings[2] = threads[2]->sibling;
    siblings[3] = threads[3]->sibling;
    siblings[4] = threads[4]->sibling;
    test_terminate_threads();
    test_wait_threads();
    test_assert(parents[0] == chThdGetSelfX() &&
                parents[2] == chThdGetSelfX() &&
                parents[3] == chThdGetSelfX() &&
                siblings[2] == threads_cpy[0] &&
                siblings[3] == threads_cpy[2], "invalid children list");
  }
  test_end_step(4);

  /* [5.5.5] Creating 5 child threads with increasing priority and then
   * increasing the priority of a low-priority child, hierarchy information is
   * tested. */
  test_set_step(5);
  {
    tprio_t prios[MAX_THREADS];
    thread_t *threads_cpy[MAX_THREADS];
    tprio_t testprios[2];
    thread_t *siblings[MAX_THREADS];
    prios[0] = chThdGetPriorityX()-5;
    prios[1] = chThdGetPriorityX()-4;
    prios[2] = chThdGetPriorityX()-3;
    prios[3] = chThdGetPriorityX()-2;
    prios[4] = chThdGetPriorityX()-1;
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, prios[0], hierarchythread, &prios[0]);
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, prios[1], hierarchythread, &prios[1]);
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, prios[2], hierarchythread, &prios[2]);
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, prios[3], hierarchythread, &prios[3]);
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, prios[4], hierarchythread, &prios[4]);
    threads_cpy[0] = threads[0];
    threads_cpy[1] = threads[1];
    threads_cpy[2] = threads[2];
    threads_cpy[3] = threads[3];
    threads_cpy[4] = threads[4];
    chThdSleepMilliseconds(CHILDTHREAD_SLEEP_MS*2);
    testprios[0] = threads[0]->hdr.pqueue.prio;
    prios[0] = prios[4];
    chThdSleepMilliseconds(CHILDTHREAD_SLEEP_MS*2);
    testprios[1] = threads[0]->hdr.pqueue.prio;
    siblings[0] = threads[0]->sibling;
    siblings[1] = threads[1]->sibling;
    siblings[2] = threads[2]->sibling;
    siblings[3] = threads[3]->sibling;
    siblings[4] = threads[4]->sibling;
    test_terminate_threads();
    test_wait_threads();
#if CH_CFG_THREADHIERARCHY_ORDERED == TRUE
    test_assert(testprios[0] == chThdGetPriorityX()-5 &&
                testprios[1] == prios[4] &&
                siblings[0] == threads_cpy[4] &&
                siblings[2] == threads_cpy[1] &&
                siblings[3] == threads_cpy[2] &&
                siblings[4] == threads_cpy[3], "invalid children list");
#else
    test_assert(testprios[0] == chThdGetPriorityX()-5 &&
                testprios[1] == prios[4] &&
                siblings[1] == threads_cpy[0] &&
                siblings[2] == threads_cpy[1] &&
                siblings[3] == threads_cpy[2] &&
                siblings[4] == threads_cpy[3], "invalid children list");
#endif
  }
  test_end_step(5);

  /* [5.5.6] Creating 5 child threads with increasing priority and then
   * decreasing the priority of a high-priority child, hierarchy information is
   * tested. */
  test_set_step(6);
  {
    tprio_t prios[MAX_THREADS];
    thread_t *threads_cpy[MAX_THREADS];
    tprio_t testprios[2];
    thread_t *siblings[MAX_THREADS];
    prios[0] = chThdGetPriorityX()-5;
    prios[1] = chThdGetPriorityX()-4;
    prios[2] = chThdGetPriorityX()-3;
    prios[3] = chThdGetPriorityX()-2;
    prios[4] = chThdGetPriorityX()-1;
    threads[0] = chThdCreateStatic(wa[0], WA_SIZE, prios[0], hierarchythread, &prios[0]);
    threads[1] = chThdCreateStatic(wa[1], WA_SIZE, prios[1], hierarchythread, &prios[1]);
    threads[2] = chThdCreateStatic(wa[2], WA_SIZE, prios[2], hierarchythread, &prios[2]);
    threads[3] = chThdCreateStatic(wa[3], WA_SIZE, prios[3], hierarchythread, &prios[3]);
    threads[4] = chThdCreateStatic(wa[4], WA_SIZE, prios[4], hierarchythread, &prios[4]);
    threads_cpy[0] = threads[0];
    threads_cpy[1] = threads[1];
    threads_cpy[2] = threads[2];
    threads_cpy[3] = threads[3];
    threads_cpy[4] = threads[4];
    chThdSleepMilliseconds(CHILDTHREAD_SLEEP_MS*2);
    testprios[0] = threads[4]->hdr.pqueue.prio;
    prios[4] = prios[0];
    chThdSleepMilliseconds(CHILDTHREAD_SLEEP_MS*2);
    testprios[1] = threads[4]->hdr.pqueue.prio;
    siblings[0] = threads[0]->sibling;
    siblings[1] = threads[1]->sibling;
    siblings[2] = threads[2]->sibling;
    siblings[3] = threads[3]->sibling;
    siblings[4] = threads[4]->sibling;
    test_terminate_threads();
    test_wait_threads();
#if CH_CFG_THREADHIERARCHY_ORDERED == TRUE
    test_assert(testprios[0] == chThdGetPriorityX()-1 &&
                testprios[1] == prios[0] &&
                siblings[1] == threads_cpy[4] &&
                siblings[2] == threads_cpy[1] &&
                siblings[3] == threads_cpy[2] &&
                siblings[4] == threads_cpy[0], "invalid children list");
#else
    test_assert(testprios[0] == chThdGetPriorityX()-1 &&
                testprios[1] == prios[0] &&
                siblings[1] == threads_cpy[0] &&
                siblings[2] == threads_cpy[1] &&
                siblings[3] == threads_cpy[2] &&
                siblings[4] == threads_cpy[3], "invalid children list");
#endif
  }
  test_end_step(6);
}

static const testcase_t rt_test_005_005 = {
#if CH_CFG_THREADHIERARCHY_ORDERED == TRUE
  "Thread hierarchy with ordered lists",
#else
  "Thread hierarchy with unordered lists",
#endif
  NULL,
  NULL,
  rt_test_005_005_execute
};
#endif /* CH_CFG_USE_THREADHIERARCHY */

/****************************************************************************
 * Exported data.
 ****************************************************************************/

/**
 * @brief   Array of test cases.
 */
const testcase_t * const rt_test_sequence_005_array[] = {
  &rt_test_005_001,
  &rt_test_005_002,
  &rt_test_005_003,
#if (CH_CFG_USE_MUTEXES == TRUE) || defined(__DOXYGEN__)
  &rt_test_005_004,
#endif
#if (CH_CFG_USE_THREADHIERARCHY == TRUE) || defined(__DOXYGEN__)
  &rt_test_005_005,
#endif
  NULL
};

/**
 * @brief   Threads Functionality.
 */
const testsequence_t rt_test_sequence_005 = {
  "Threads Functionality",
  rt_test_sequence_005_array
};

#if (CH_CFG_USE_THREADHIERARCHY == TRUE)
#undef CHILDTHREAD_SLEEP_MS
#endif
