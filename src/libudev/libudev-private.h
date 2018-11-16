/* SPDX-License-Identifier: LGPL-2.1+ */
#pragma once

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#include "libudev.h"
#include "sd-device.h"

#include "macro.h"
#include "mkdir.h"
#include "strxcpyx.h"
#include "util.h"

/* libudev-device.c */
struct udev_device *udev_device_new_from_nulstr(struct udev *udev, char *nulstr, ssize_t buflen);
struct udev_device *udev_device_new_from_synthetic_event(struct udev *udev, const char *syspath, const char *action);
struct udev_device *udev_device_shallow_clone(struct udev_device *old_device);
struct udev_device *udev_device_clone_with_db(struct udev_device *old_device);
int udev_device_copy_properties(struct udev_device *dst, struct udev_device *src);
mode_t udev_device_get_devnode_mode(struct udev_device *udev_device);
uid_t udev_device_get_devnode_uid(struct udev_device *udev_device);
gid_t udev_device_get_devnode_gid(struct udev_device *udev_device);
int udev_device_rename(struct udev_device *udev_device, const char *new_name);
int udev_device_add_devlink(struct udev_device *udev_device, const char *devlink);
void udev_device_cleanup_devlinks_list(struct udev_device *udev_device);
int udev_device_add_property(struct udev_device *udev_device, const char *key, const char *value);
char **udev_device_get_properties_envp(struct udev_device *udev_device);
ssize_t udev_device_get_properties_monitor_buf(struct udev_device *udev_device, const char **buf);
const char *udev_device_get_devpath_old(struct udev_device *udev_device);
const char *udev_device_get_id_filename(struct udev_device *udev_device);
void udev_device_set_is_initialized(struct udev_device *udev_device);
int udev_device_add_tag(struct udev_device *udev_device, const char *tag);
void udev_device_remove_tag(struct udev_device *udev_device, const char *tag);
void udev_device_cleanup_tags_list(struct udev_device *udev_device);
void udev_device_ensure_usec_initialized(struct udev_device *udev_device, struct udev_device *old_device);
int udev_device_get_devlink_priority(struct udev_device *udev_device);
int udev_device_set_devlink_priority(struct udev_device *udev_device, int prio);
int udev_device_get_watch_handle(struct udev_device *udev_device);
int udev_device_set_watch_handle(struct udev_device *udev_device, int handle);
int udev_device_get_ifindex(struct udev_device *udev_device);
void udev_device_set_info_loaded(struct udev_device *device);
void udev_device_set_db_persist(struct udev_device *udev_device);
void udev_device_read_db(struct udev_device *udev_device);
int udev_device_update_db(struct udev_device *udev_device);
int udev_device_delete_db(struct udev_device *udev_device);
int udev_device_tag_index(struct udev_device *dev, struct udev_device *dev_old, bool add);

/* libudev-monitor.c - netlink/unix socket communication  */
int udev_monitor_disconnect(struct udev_monitor *udev_monitor);
int udev_monitor_allow_unicast_sender(struct udev_monitor *udev_monitor, struct udev_monitor *sender);
int udev_monitor_send_device(struct udev_monitor *udev_monitor,
                             struct udev_monitor *destination, struct udev_device *udev_device);
struct udev_monitor *udev_monitor_new_from_netlink_fd(struct udev *udev, const char *name, int fd);
int udev_monitor_receive_sd_device(struct udev_monitor *udev_monitor, sd_device **ret);

/* libudev-list.c */
struct udev_list_node {
        struct udev_list_node *next, *prev;
};
struct udev_list {
        struct udev *udev;
        struct udev_list_node node;
        struct udev_list_entry **entries;
        unsigned entries_cur;
        unsigned entries_max;
        bool unique;
};
void udev_list_node_init(struct udev_list_node *list);
int udev_list_node_is_empty(struct udev_list_node *list);
void udev_list_node_append(struct udev_list_node *new, struct udev_list_node *list);
void udev_list_node_remove(struct udev_list_node *entry);
#define udev_list_node_foreach(node, list) \
        for (node = (list)->next; \
             node != list; \
             node = (node)->next)
#define udev_list_node_foreach_safe(node, tmp, list) \
        for (node = (list)->next, tmp = (node)->next; \
             node != list; \
             node = tmp, tmp = (tmp)->next)
void udev_list_init(struct udev *udev, struct udev_list *list, bool unique);
void udev_list_cleanup(struct udev_list *list);
struct udev_list_entry *udev_list_get_entry(struct udev_list *list);
struct udev_list_entry *udev_list_entry_add(struct udev_list *list, const char *name, const char *value);
void udev_list_entry_delete(struct udev_list_entry *entry);
int udev_list_entry_get_num(struct udev_list_entry *list_entry);
void udev_list_entry_set_num(struct udev_list_entry *list_entry, int num);
#define udev_list_entry_foreach_safe(entry, tmp, first) \
        for (entry = first, tmp = udev_list_entry_get_next(entry); \
             entry != NULL; \
             entry = tmp, tmp = udev_list_entry_get_next(tmp))

/* libudev-util.c */
#define UTIL_PATH_SIZE                      1024
#define UTIL_NAME_SIZE                       512
#define UTIL_LINE_SIZE                     16384
#define UDEV_ALLOWED_CHARS_INPUT        "/ $%?,"
size_t util_path_encode(const char *src, char *dest, size_t size);
int util_replace_whitespace(const char *str, char *to, size_t len);
int util_replace_chars(char *str, const char *white);
uint32_t util_string_hash32(const char *key);
uint64_t util_string_bloom64(const char *str);
int util_resolve_subsys_kernel(const char *string, char *result, size_t maxsize, int read_value);

/* Cleanup functions */
DEFINE_TRIVIAL_CLEANUP_FUNC(struct udev*, udev_unref);
DEFINE_TRIVIAL_CLEANUP_FUNC(struct udev_device*, udev_device_unref);
DEFINE_TRIVIAL_CLEANUP_FUNC(struct udev_enumerate*, udev_enumerate_unref);
DEFINE_TRIVIAL_CLEANUP_FUNC(struct udev_monitor*, udev_monitor_unref);
DEFINE_TRIVIAL_CLEANUP_FUNC(struct udev_hwdb*, udev_hwdb_unref);
DEFINE_TRIVIAL_CLEANUP_FUNC(struct udev_queue*, udev_queue_unref);
