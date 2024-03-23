## Decoupling Persistent Bucket Notifications from RADOS Backend

### Summary

This project aims to decouple the persistent bucket notifications in Ceph's RADOS Gateway (RGW) from the RADOS backend by introducing an abstraction layer for the notification queue. This will enable the implementation of alternative backends while maintaining the reliability and functionality of the notification system. This project will use Redis Queue to implement the abstract layer.

### Benefits for Ceph

- By abstracting the notification queue and lock mechanisms away from RADOS-specific implementations (`cls_2pc_queue` and `cls_lock`), bucket notifications can be supported even when using non-RADOS storage backends like POSIX, DBStore, DAOS etc when combined with the RGW Zipper project.
- Removing the dependency on RADOS will reduce the load on the RADOS cluster from the persistent notification feature.
- Introducing a queue abstraction layer will make the RGW notification code cleaner and easier to maintain by separating out the queue operations from the RADOS object operations in `rgw_notify`.


### The Project

1. The current notification queue is backed by a custom `cls` queue module (`cls_queue`) which registers several methods to operate on RADOS objects: `init`, `get_capacity`, `enqueue`, `list_entries`, `remove_entries`. The Redis Queue implementation (using Boost.Redis) should also support these basic functionalities.

2. To achieve a distributed reliable message queue, RGW implements a 2 Phase Commit Queue (`cls_2pc_queue`) on top of `cls_queue`. Pushing a notification to this queue involves two steps:
   1. Reserve a spot for the notification in the queue. The reserved data is stored in a specific buffer in the queue header (`cls_queue_head`).
   2.  If everything goes well, commit the notification and the data enters the queue from the header buffer. If any RADOS operation fails, abort the notification.<br>
      The introduced abstraction layer should support reserve, commit and abort operations, so that `cls_2pc_queue` and `rgw_rados_operate` related code in `rgw_notify` can be replaced with methods from the abstraction layer.

3. One of the RGWs will asynchronously list the notifications in the queue and send them. If an ack is received from the endpoint, the notification is removed from the queue. Unacked notifications remain in the queue for retry. For the Redis Queue implementation, one possible approach is to use a Redis Set to store reserved messages. For messages to be committed, they can be taken out from the Set and pushed into a Redis Queue for sending.

4. After implementing the abstraction layer with Redis, messages should be persisted using RADOS or other storage backends. A config option is needed to select the implementation to be used at runtime.

#### Test Plan

1. Write a test similar to `test_cls_2pc_queue` to verify the functionality of the abstraction layer is correct. The abstraction layer needs to support the essential features of the existing cls_2pc_queue.
2. After integrating the abstraction layer into `rgw_notify`, ensure it can pass the existing `rgw/bucket_notification/` tests.

### Required Deliverables

1. `rgw_notify code` should be refactored to remove dependency on RADOS, using an abstraction layer to achieve existing functionality.
2. An abstraction layer that can be implemented using either `cls_2pc_queue` or Boost.Redis.
3. A config option to select the implementation of the abstraction layer at runtime.
4. A test and setup instructions when using the RADOS backend.
5. Regression tests passed.

### Stretch Goal

1. An abstract layer for `cls_lock`
2. Move `rgw_notify` outside of the `driver/rados` directory
3. A test and setup instructions when using a non-RADOS backend.