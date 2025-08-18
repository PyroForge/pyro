#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <random>
#include <chrono>
std::pair<std::string, std::string> sp(const std::string& input) {
    size_t colonPos = input.find(':');
    size_t semicolonPos = input.find(';');
    size_t splitPos = std::string::npos;
    if (colonPos != std::string::npos && semicolonPos != std::string::npos) {
        splitPos = std::min(colonPos, semicolonPos);
    } else if (colonPos != std::string::npos) {
        splitPos = colonPos;
    } else if (semicolonPos != std::string::npos) {
        splitPos = semicolonPos;
    }
    if (splitPos != std::string::npos) {
        std::string before = input.substr(0, splitPos);
        std::string after = input.substr(splitPos + 1);
        return {before, after};
    } else {
        // No delimiter found: return whole string as "before", empty string as "after"
        return {input, ""};
    }
}
std::string generate_random_string(int length) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::mt19937 engine(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(0, characters.size() - 1);
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += characters[dist(engine)];
    }
    return result;
}
void native_functions(std::vector<std::string>& og,std::vector<std::string>& vars, int counter, std::string& tab, std::string& topL64, std::string& bssL64, std::string& dataL64, std::string& textL64, std::string& topA64, std::string& bssA64, std::string& dataA64, std::string& textA64, std::string& len_key, std::string& v_key, std::string& f_key) {
    auto instruction = sp(og[counter]);
    // W end:{exit code}
    if (instruction.first == "n|end" or instruction.first == "end") {
        // linux
        textL64 = textL64 + tab + "mov rax,60\n";
        textL64 = textL64 + tab + "xor rdi,rdi\n";
        textL64 = textL64 + tab + "syscall\n";
        // linux arm
        textA64 = textA64 + tab + "mov	x0, #0\n";
        textA64 = textA64 + tab + "mov	x8, #93\n";
        textA64 = textA64 + tab + "svc	#0\n";
     }
    // W syscall:
    if (instruction.first == "n|syscall" or instruction.first == "syscall") {
        // linux
        textL64 = textL64 + tab + "syscall\n";
        // linux arm
        textA64 = textA64 + tab + "svc	#0\n";
    }
    // W set:{type}:{varname}:{desired contents}
    if (instruction.first == "n|decl" or instruction.first == "decl") {
        // this is not really good code down here but I'll fix it later
        auto set1 = sp(instruction.second);
        auto set2 = sp(set1.second);
        std::string types = set1.first;
        std::string varname = set2.first;
        std::string varE = set2.second;
        vars.push_back(varname);
        if (types == "str") {
            dataL64 = dataL64 + tab + varname + " db " + varE + "\n";
            dataA64 = dataA64 + tab + varname + ": .asciz " + varE + "\n";
        }
        if (types == "int") {
            dataL64 = dataL64 + tab + varname + " dq " + varE + "\n";
            dataA64 = dataA64 + tab + varname + ": .quad " + varE + "\n";
        }
    }
    // W add:{product}:{var1}:{var2}
    if (instruction.first == "n|add" or instruction.first == "add") {
        // this is not really good code down here but I'll fix it later
        auto set1 = sp(instruction.second);
        auto set2 = sp(set1.second);
        std::string save = set1.first;
        std::string var1 = set2.first;
        std::string var2 = set2.second;
        // linux
        textL64 = textL64 + tab + "mov rax,[" + var1 + "]\n";
        textL64 = textL64 + tab + "mov rbx,[" + var2 + "]\n";
        textL64 = textL64 + tab + "add rax,rbx\n";
        textL64 = textL64 + tab + "mov [" + save + "],rax\n";
        // linux arm
        // old way
        //textA64 = textA64 + tab + "ldr x0, =" + var1 + "\n";
        //textA64 = textA64 + tab + "ldr x1, [x0]\n";
        //textA64 = textA64 + tab + "ldr x2, =" + var2 + "\n";
        //textA64 = textA64 + tab + "ldr x3, [x2]\n";
        // add
        //textA64 = textA64 + tab + "ldr x5, =" + save + "\n";
        //textA64 = textA64 + tab + "str x4, [x5]\n";
        textA64 = textA64 + tab + "adrp x0," + var1 + "\n";
        textA64 = textA64 + tab + "add  x0, x0, :lo12:" + var1 + "\n";
        textA64 = textA64 + tab + "ldr  x1, [x0]\n";
        textA64 = textA64 + tab + "adrp x2," + var2 + "\n";
        textA64 = textA64 + tab + "add  x2, x2, :lo12:" + var2 + "\n";
        textA64 = textA64 + tab + "ldr  x3, [x2]\n";
        textA64 = textA64 + tab + "add x4,x1,x3\n";
        textA64 = textA64 + tab + "adrp x5," + save + "\n";
        textA64 = textA64 + tab + "add  x5,x5,:lo12:" + save + "\n";
        textA64 = textA64 + tab + "str  x4,[x5]" + "\n";
    }
    // W sub:{product}:{var1}:{var2}
    if (instruction.first == "n|sub" or instruction.first == "sub") {
        // this is not really good code down here but I'll fix it later
        auto set1 = sp(instruction.second);
        auto set2 = sp(set1.second);
        std::string save = set1.first;
        std::string var1 = set2.first;
        std::string var2 = set2.second;
        // linux
        textL64 = textL64 + tab + "mov rax,[" + var1 + "]\n";
        textL64 = textL64 + tab + "mov rbx,[" + var2 + "]\n";
        textL64 = textL64 + tab + "sub rax,rbx\n";
        textL64 = textL64 + tab + "mov [" + save + "],rax\n";
        // linux arm
        // old way
        //textA64 = textA64 + tab + "ldr x0, =" + var1 + "\n";
        //textA64 = textA64 + tab + "ldr x1, [x0]\n";
        //textA64 = textA64 + tab + "ldr x2, =" + var2 + "\n";
        //textA64 = textA64 + tab + "ldr x3, [x2]\n";
        // sub
        //textA64 = textA64 + tab + "ldr x5, =" + save + "\n";
        //textA64 = textA64 + tab + "str x4, [x5]\n";
        textA64 = textA64 + tab + "adrp x0," + var1 + "\n";
        textA64 = textA64 + tab + "add  x0, x0, :lo12:" + var1 + "\n";
        textA64 = textA64 + tab + "ldr  x1, [x0]\n";
        textA64 = textA64 + tab + "adrp x2," + var2 + "\n";
        textA64 = textA64 + tab + "add  x2, x2, :lo12:" + var2 + "\n";
        textA64 = textA64 + tab + "ldr  x3, [x2]\n";
        textA64 = textA64 + tab + "sub x4,x1,x3\n";
        textA64 = textA64 + tab + "adrp x5," + save + "\n";
        textA64 = textA64 + tab + "add  x5,x5,:lo12:" + save + "\n";
        textA64 = textA64 + tab + "str  x4,[x5]" + "\n";
    }
    // log:{type}:{"text" or var}
    if (instruction.first == "n|log" or instruction.first == "log") {
        auto set1 = sp(instruction.second);
        std::string types = set1.first;
        std::string varr = set1.second;
        if (types == "dstr") {
            // linux
            dataL64 = dataL64 + tab + v_key + std::to_string(counter) + ": db " + set1.second + "\n";
            dataL64 = dataL64 + tab + len_key + std::to_string(counter) + ": equ $ - " + v_key + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "mov rax,1\n";
            textL64 = textL64 + tab + "mov rdi,1\n";
            textL64 = textL64 + tab + "mov rsi," + v_key + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "mov rdx," + len_key + std::to_string(counter) + "\n";
            textL64 = textL64 + tab + "syscall\n";
            // linux arm
            dataA64 = dataA64 + tab + v_key + std::to_string(counter) + ": .asciz " + set1.second + "\n";
            dataA64 = dataA64 + tab + len_key + std::to_string(counter) + " = . - " + v_key + std::to_string(counter) + "\n";
            textA64 = textA64 + tab + "mov	x8, #64\n";
            textA64 = textA64 + tab + "mov	x0, #1\n";
            textA64 = textA64 + tab + "ldr	x1, =" + v_key + std::to_string(counter) + "\n";
            textA64 = textA64 + tab + "mov	x2, " + len_key + std::to_string(counter) + "\n";
            textA64 = textA64 + tab + "svc	#0\n";
        }
        if (types == "str") {}
        if (types == "int") {}
    }
    // W raw_log:{"text"}
    if (instruction.first == "raw_log") {
        // linux
        dataL64 = dataL64 + tab + v_key + std::to_string(counter) + ": db " + instruction.second + "\n";
        dataL64 = dataL64 + tab + len_key + std::to_string(counter) + ": equ $ - " + v_key + std::to_string(counter) + "\n";
        textL64 = textL64 + tab + "mov rax,1\n";
        textL64 = textL64 + tab + "mov rdi,1\n";
        textL64 = textL64 + tab + "mov rsi," + v_key + std::to_string(counter) + "\n";
        textL64 = textL64 + tab + "mov rdx," + len_key + std::to_string(counter) + "\n";
        textL64 = textL64 + tab + "syscall\n";
        // linux arm
        dataA64 = dataA64 + tab + v_key + std::to_string(counter) + ": .asciz " + instruction.second + "\n";
        dataA64 = dataA64 + tab + len_key + std::to_string(counter) + " = . - " + v_key + std::to_string(counter) + "\n";
        textA64 = textA64 + tab + "mov	x8, #64\n";
        textA64 = textA64 + tab + "mov	x0, #1\n";
        textA64 = textA64 + tab + "ldr	x1, =" + v_key + std::to_string(counter) + "\n";
        textA64 = textA64 + tab + "mov	x2, " + len_key + std::to_string(counter) + "\n";
        textA64 = textA64 + tab + "svc	#0\n";
    }
    // NW input:{var}:{read byts}
    if (instruction.first == "input") {
        auto set1 = sp(instruction.second);
        //linux
        textL64 = textL64 + tab + "mov rax,0\n";
        textL64 = textL64 + tab + "mov rdi,0\n";
        textL64 = textL64 + tab + "mov rsi," + set1.first + "\n";
        textL64 = textL64 + tab + "mov rdx," + set1.second + "\n";
        textL64 = textL64 + tab + "syscall\n";
        //linux arm NOT WORKING
        textA64 = textA64 + tab + "mov x7,#3\n";
        textA64 = textA64 + tab + "mov x0,#0\n";
        textA64 = textA64 + tab + "ldr x1, =" + set1.first + "\n";
        textA64 = textA64 + tab + "mov x2, #" + set1.second + "\n";
        textA64 = textA64 + tab + "svc #0\n";
    }
    // WORKING
    if (instruction.first == "back") {
        auto set1 = sp(instruction.second);
        std::string lb = set1.first;
        std::string ab = set1.second;
        //linux
        textL64 = textL64 + tab + "jmp near -" + lb + "\n";
        //linux arm
        textA64 = textA64 + tab + "b #-" + ab + "\n";
    }
}
int main(int argc, char* argv[]) {
    if (argc == 1 or argc == 2) {
        std::cout << "involved use ./pyro {inputfile} {outputfile} {flags}";
        return EXIT_FAILURE;
    }
    // config
    std::string output = argv[2];
    std::string tab = "    ";
    std::string len_key = generate_random_string(10);
    std::string v_key = generate_random_string(10);
    std::string f_key = generate_random_string(10);
    //set up vars
    // linux 64bit x86_64 nasm
    std::string topL64 = ";";
    std::string bssL64 = "section .bss\n";
    std::string dataL64 = "section .data\n";
    std::string textL64 = "section .text\n" + tab + "global _start\n_start:\n";
    // linux 64bit arm
    std::string topA64 = ";";
    std::string bssA64 = ".section .bss\n";
    std::string dataA64 = ".section .data\n";
    std::string textA64 = ".section .text\n" + tab + ".global _start\n_start:\n";
    // open file and puting it in list og
    std::ifstream file(argv[1]);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string preog = buffer.str();
    file.close();
    std::istringstream iss(preog);
    std::string linee;
    std::vector<std::string> og;
    std::vector<std::string> vars;
    vars.push_back("varnull");
    while (std::getline(iss, linee)) {
        og.push_back(linee);
    }
    int counter = 0;
    while (counter < og.size()) {
        native_functions(og, vars, counter, tab, topL64, bssL64, dataL64, textL64, topA64, bssA64, dataA64, textA64, len_key, v_key, f_key);
        counter++;
    }
    std::ofstream L64(output + ".asm");
    L64 << topL64 + "\n";
    L64 << bssL64 + "\n";
    L64 << dataL64 + "\n";
    L64 << textL64 + "\n";
    L64.close();
    std::ofstream A64(output + ".s");
    A64 << topA64 + "\n";
    A64 << bssA64 + "\n";
    A64 << dataA64 + "\n";
    A64 << textA64 + "\n";
    A64.close();
    return EXIT_SUCCESS;
}
