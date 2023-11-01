#include "Process.h"
#include "utils.h"


// 将 element_t 转换为字符串
std::string element_to_string(element_t element) {
    size_t length = element_length_in_bytes(element);   //128 bit
    // std::cout << "(element_to_string): element_t_length: " << length
    //           << std::endl;
    uint8_t* bytes = (uint8_t*)malloc(length * sizeof(uint8_t));
    element_to_bytes(bytes, element);
    char* res = (char*)malloc((2 * length + 1) * sizeof(char));

    for (size_t i = 0; i < length; ++i) {
        sprintf(&res[2 * i], "%02X", bytes[i]);
    }

    free(bytes);

    return std::string(res);
}

// 将字符串转换为 element_t
void string_to_element(element_t element, const std::string& str) {
    char* str_tmp = const_cast<char*>(str.c_str());
    size_t length = strlen(str_tmp) / 2;
    uint8_t* bytes = (uint8_t*)malloc(length * sizeof(uint8_t));
    for (size_t i = 0; i < length; ++i) {
        sscanf(&str_tmp[2 * i], "%02X", &bytes[i]);
    }
    element_from_bytes(element, bytes);
    free(bytes);
}

void pairing_to_buffer(pairing_s *pair, uint8_t **buffer, size_t *size) {
    // 计算所需的字节流长度
    size_t byte_count = sizeof(pairing_s);
    std::cout << "pairing_s-byte_count: " << byte_count << std::endl;

    // 分配内存
    *buffer = (uint8_t *)malloc(byte_count);
    if (*buffer == NULL) {
        // 内存分配失败的错误处理
        return;
    }

    // 将结构体实例的内存复制到字节流缓冲区
    memcpy(*buffer, pair, byte_count);

    // 设置字节流的大小
    *size = byte_count;
}

void deserialize_pairing(pairing_s *pair, unsigned char *buffer, size_t size) {
    // 检查字节流大小是否与结构体大小匹配
    if (size != sizeof(pairing_s)) {
        // 字节流大小不匹配的错误处理
        // ...
        return;
    }

    // 将字节流的内容复制到结构体实例的内存
    memcpy(pair, buffer, size);
}

int main() {
    const char *path = "../param/a.param";
    
    // [1] Setup, system parameters generation
    std::cout << "\n-----[1] Setup, system parameters generation-----"
              << std::endl;
    KGC *kgc_ptr_1 = new KGC(path);

    element_printf("P = %B\n", kgc_ptr_1->P);
    element_printf("P0 = %B\n", kgc_ptr_1->P0);


    element_t GT1, GT2;
    element_init_GT(GT1, kgc_ptr_1->e);
    pairing_apply(GT1, kgc_ptr_1->P, kgc_ptr_1->P0, kgc_ptr_1->e);
    element_printf("GT1: %B\n", GT1);
    // Element_resotre PART
    // std::string P_str = element_to_string(kgc->P);
    // element_t P_restore;
    // std::cout << "P_str: " << P_str << std::endl;

    // element_init_G1(P_restore, kgc->e);
    // string_to_element(P_restore, P_str);    
    // element_printf("P_restore = %B\n", P_restore);

    // element_clear(P_restore); 

    
    // pairing_s e_tmp = *(kgc_ptr_1->e);

    // 

    pairing_s e_tmp = *(kgc_ptr_1->e);

    // pairing_t e_copy;
    // e_copy[0] = e_tmp;

    // // std::cout << "e_copy_size: " << sizeof(e_copy);

    // element_init_GT(GT2, e_copy);
    // pairing_apply(GT2, kgc_ptr_1->P, kgc_ptr_1->P0, e_copy);
    // element_printf("GT2: %B\n", GT2);

    uint8_t *buffer;
    size_t size;
    pairing_to_buffer(&e_tmp, &buffer, &size);
    std::cout << "size: " << size << std::endl;
    // for(int i = 0; i < size; i++) {
    //     std::cout << int(buffer[i]) << " ";
    // }
    // std::cout << std::endl;


    return 0;
}