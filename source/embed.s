    .section .rodata
    .global gtaPayload
    .type   gtaPayload, @object
    .align  4
gtaPayload:
    .incbin "gtaPayload/gtaPayload.bin"
gtaPayloadEnd:
    .global gtaPayloadSize
    .type   gtaPayloadSize, @object
    .align  4
gtaPayloadSize:
    .int    gtaPayloadEnd - gtaPayload
