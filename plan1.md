# draw handle + adapter + bsp driver

1，如果没有上下文，请先阅读claude.md 以此了解项目结构和一些准则

2，文档编码格式请参考根目录下的.clang-format文件

---

## 架构设计

遵循现有分层模式：**Handle → Adapter → Bsp_driver**

```
draw_handle (Handle/)
    ↓ 依赖 draw_handle_ops_t 函数指针表
draw_adapter (Adapter/)
    ↓ 封装 void* → st7789_driver_t* 类型转换
bsp_drv_st7789 (Bsp_drivers/)  ← 已完成，无需修改
```

### 功能范围（基础功能）

| 函数 | 说明 |
|------|------|
| `fill_rect` | 矩形填充 |
| `draw_line` | 直线绘制（Bresenham） |
| `draw_string` | 字符串绘制 |

---

## 任务1

在 Handle/ 和 Adapter/ 文件夹下创建源文件和头文件，添加进对应的 CMakeLists.txt。

### 新建文件清单

| 文件 | 路径 |
|------|------|
| `draw_adapter.h` | `Adapter/Inc/draw_adapter.h` |
| `draw_adapter.c` | `Adapter/Src/draw_adapter.c` |
| `draw_handle.h` | `Handle/Inc/draw_handle.h` |
| `draw_handle.c` | `Handle/Src/draw_handle.c` |

### CMakeLists.txt 修改

- `Adapter/CmakeLists.txt`：在 `ADAPTER_SRC` 中添加 `Src/draw_adapter.c`
- `Handle/CMakeLists.txt`：在 `HANDLE_SRC` 中添加 `Src/draw_handle.c`

---

## 任务2

编写 draw_adapter 和 draw_handle 的代码桥接。

### draw_adapter.h 接口

```c
uint8_t drv_draw_init(void *p_drv, void *drv_ops);
uint8_t drv_draw_fill_rect(void *p_drv, uint16_t x, uint16_t y,
                           uint16_t w, uint16_t h, uint16_t color);
uint8_t drv_draw_line(void *p_drv, uint16_t x0, uint16_t y0,
                      uint16_t x1, uint16_t y1, uint16_t color);
uint8_t drv_draw_string(void *p_drv, const front_def_t *p_font,
                        uint16_t x, uint16_t y, const char *p_str,
                        uint16_t f_color, uint16_t b_color);
```

### draw_adapter.c 实现

- 每个函数入口检查 `p_drv != NULL`
- `void *p_drv` 强制转换为 `st7789_driver_t *`
- 调用对应的 `p_drv->pf_xxx(...)` 函数指针
- 返回值：0=成功, 1=NULL参数, 2=驱动错误
- `drv_draw_init` 调用 `st7789_driver_instruct()`

### draw_handle.h 接口

```c
typedef struct {
    uint8_t (*pf_drv_init)(void *p_drv, void *drv_ops);
    uint8_t (*pf_fill_rect)(void *p_drv, uint16_t x, uint16_t y,
                            uint16_t w, uint16_t h, uint16_t color);
    uint8_t (*pf_draw_line)(void *p_drv, uint16_t x0, uint16_t y0,
                            uint16_t x1, uint16_t y1, uint16_t color);
    uint8_t (*pf_draw_string)(void *p_drv, const front_def_t *p_font,
                              uint16_t x, uint16_t y, const char *p_str,
                              uint16_t f_color, uint16_t b_color);
} draw_handle_ops_t;

typedef struct {
    const draw_handle_ops_t *p_ops;
    void                    *p_drv;
} draw_handle_t;
```

### draw_handle.c 实现

- `draw_handle_fill_rect()` / `draw_handle_draw_line()` / `draw_handle_draw_string()`：薄封装，调用 `p_handle->p_ops->pf_xxx(p_handle->p_drv, ...)`
- `draw_handle_instruct()`：绑定 ops + drv，调用 `pf_drv_init()`

---

## 任务3

编译通过后在 freertos.c 中添加简单测试：

1. 定义 `draw_handle_ops_t g_draw_adapter_ops` 绑定 adapter 函数
2. 定义 `draw_handle_t g_draw_handle`
3. 在 `StartDefaultTask` 中调用 `draw_handle_instruct()` 初始化
4. 使用 handle API 绘制测试内容（替代部分现有的直接调用 `g_st7789_drv.pf_xxx`）
5. 编译通过即验证链路通畅
