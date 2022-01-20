# VS Code debug

```
*((uint8_t*)SP)@25
*((uint8_t*)_k_stack_buf_t1)@256
*((uint8_t*)_k_main_stack)@512
*((uint8_t*)(0xea + 1))@21

*((_k_callsaved_ctx*)0x23a)

(struct _k_callsaved_ctx*)(_k_main_stack + 0x200 - sizeof(struct _k_callsaved_ctx) - 4)
(struct _k_callsaved_ctx*)(0x100 + 0x200 - sizeof(struct _k_callsaved_ctx) - 4)
```



*((uint8_t*)(0x1b4))@10