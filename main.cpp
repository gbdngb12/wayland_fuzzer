#include <stdio.h>
#include <wayland-client.h>
#include "protocol.h"
#include <stdbool.h>
#include <string>
#include <vector>
#include <random>
// #include <iostream>
// global singleton object
// wl_display, wl_registry, wl_compositor, wl_shm, wl_data_device_manager 


#define OBJECT_POOL_SIZE 100

// 자동화 필요
std::vector<const struct wl_interface*> interface_list {
    &wl_display_interface,
    &wl_registry_interface,
    &wl_callback_interface,
    &wl_compositor_interface,
    &wl_shm_pool_interface,
    &wl_shm_interface,
    &wl_buffer_interface,
    &wl_data_offer_interface,
    &wl_data_source_interface,
    &wl_data_device_interface,
    &wl_data_device_manager_interface,
    &wl_shell_interface,
    &wl_shell_surface_interface,
    &wl_surface_interface,
    &wl_seat_interface,
    &wl_pointer_interface,
    &wl_keyboard_interface,
    &wl_touch_interface,
    &wl_output_interface,
    &wl_region_interface,
    &wl_subcompositor_interface,
    &wl_subsurface_interface
};

typedef struct WaylandObject {
    bool singleton;
    std::string object_name;
    struct wl_interface* interface;
} WaylandObject;

WaylandObject wayland_object_pool[OBJECT_POOL_SIZE];


/////////////////////////////////
// `error` 이벤트 핸들러
static void handle_display_error(void *data, struct wl_display *p_display, void* object_id, uint32_t code, const char *message) {
    fprintf(stderr, "Wayland display error: %s(%d)\n", message, code);
    exit(1);
    /**
     * 
        <enum name="error">
      <description summary="global error values">
	These errors are global and can be emitted in response to any
	server request.
      </description>
      <entry name="invalid_object" value="0"
	     summary="server couldn't find object"/>
      <entry name="invalid_method" value="1"
	     summary="method doesn't exist on the specified interface or malformed request"/>
      <entry name="no_memory" value="2"
	     summary="server is out of memory"/>
      <entry name="implementation" value="3"
	     summary="implementation error in compositor"/>
    </enum>
    */
}

// `delete_id` 이벤트 핸들러
static void handle_display_delete_id(void *data, struct wl_display *p_display, uint32_t deleted_id) {
    // ID 삭제 처리 로직
    printf("Deleted Wayland object ID: %u\n", deleted_id);
}

static const struct wl_display_listener display_listener = {
    .error = handle_display_error,
    .delete_id = handle_display_delete_id,
};

struct wl_display* new_wl_display() {
    struct wl_display* display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Cannot connect to Wayland display\n");
        return NULL;
    }
    // register listerner
    wl_display_add_listener(display, &display_listener, NULL);
    return display;
}

//////////////////////////////////

static void handle_callback_done(void* data, struct wl_callback *p_callback, uint32_t callback_data) {
    printf("callback done object callback_data : %d\n", callback_data);
}

static const struct wl_callback_listener callback_listener = {
    .done = handle_callback_done
};

struct wl_callback* new_wl_callback(struct wl_display* p_display, struct wl_surface* p_surface) {
    // 방법 1 wl_display_sync로 만드는 방법
    struct wl_callback* callback = NULL;
    if((p_display != NULL) && (p_surface == NULL)) {
        callback = wl_display_sync(p_display);
    } else if((p_display == NULL) && (p_surface != NULL)) {
        callback = wl_surface_frame(p_surface);
    } else {
        printf("new_wl_callback error\n");
        return NULL;
    }
    if (!callback) {
        fprintf(stderr, "Cannot connect to Wayland display\n");
        return NULL;
    }
    // register listener
    wl_callback_add_listener(callback, &callback_listener, NULL);  
    return callback;
}
////////////////////////////////////////
// 주어진 최대 값(maxValue) 이하의 랜덤한 정수를 반환하는 함수
int get_random_number(int max_value) {
    // 난수 생성기 초기화
    std::random_device rd;  // 하드웨어 기반의 난수 생성기를 사용하여 시드(seed)를 제공합니다.
    std::mt19937 gen(rd()); // Mersenne Twister 난수 생성 알고리즘을 사용합니다.
    std::uniform_int_distribution<> dis(0, max_value); // 균일 분포 범위 설정

    return dis(gen); // 0에서 maxValue 사이의 랜덤한 수 반환
}

int main() {
    // init interface_list
    printf("size: %ld\n", interface_list.size());

    // 벡터에서 랜덤 인덱스 선택
    int random_interface = get_random_number(interface_list.size() - 1);

    // 선택된 인터페이스 포인터 가져오기
    const struct wl_interface* selected_interface = interface_list[random_interface];
    printf("random selected interface :%s\n", selected_interface->name);
    // 현재 선택된 인터페이스가 기존 pool에 존재하는지 확인
    // 여기서 존재하는데, global singleton 객체이면 그냥 스킵


    //selected_interface->method_count 존재
    int random_method = get_random_number(selected_interface->method_count - 1);
    auto selected_method = selected_interface->methods[random_method];
    printf("random selected method : %s\n", selected_method.name);
}