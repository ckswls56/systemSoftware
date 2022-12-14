#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
using namespace std;
#define MAXSIZE 4096 // 2^12 memory maxsize

// input char c return Hex string
string hexToBinary(char c)
{
    switch (c)
    {
    case '0':
        return ("0000");

    case '1':
        return ("0001");

    case '2':
        return ("0010");

    case '3':
        return ("0011");

    case '4':
        return ("0100");

    case '5':
        return ("0101");

    case '6':
        return ("0110");

    case '7':
        return ("0111");

    case '8':
        return ("1000");

    case '9':
        return ("1001");

    case 'a':
    case 'A':
        return ("1010");

    case 'b':
    case 'B':
        return ("1011");

    case 'c':
    case 'C':
        return ("1100");

    case 'd':
    case 'D':
        return ("1101");

    case 'e':
    case 'E':
        return ("1110");

    case 'f':
    case 'F':
        return ("1111");

    default:
        return ("0000");
    }
}
// input string s return Binary string
string hexToBinary(string s)
{
    // 프로그램 입력용
    string res;

    for (int i = 0; i < s.length(); i++)
    {
        res.append(hexToBinary(s[i]));
    }
    return res;
}

// input int n return string Binary
string decimalToBinary(int n, int s = 7)
{
    string res;
    unsigned int d = 1 << s;
    while (d)
    {
        if (n & d)
            res.append("1");
        else
            res.append("0");
        d = d >> 1;
    }
    return res;
}
// input int n return Hex String
string decimalToHex(int n)
{
    string res;
    // Hex to String
    stringstream convert_invert;
    convert_invert << uppercase << hex << n;
    res = convert_invert.str();
    return res;
}
// input string s return Decimal int
int binaryToDecimal(string s)
{
    int j = s.size();
    int res = 0;
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == '1')
        {
            res += pow(2, j - 1);
        }
        j--;
    }
    return res;
}

class MEM
{
private:
    string mem[MAXSIZE];
    unsigned int mpt; // memory pointer

public:
    MEM()
    {
        resetMemAll();
        setPointer(0);
    };
    // input int n return Opcode string ex) LDA -> "0000"
    string getOpcode(int n)
    {
        string res;
        switch (n)
        {
        // LDA
        case 0:
            res = "0000";
            break;
        // STA
        case 1:
            res = "0001";
            break;
        // ADD
        case 2:
            res = "0010";
            break;
        // MUL
        case 3:
            res = "0011";
            break;
        // DIV
        case 4:
            res = "0100";
            break;
        // MOD
        case 5:
            res = "0101";
            break;
        // SEA
        case 6:
            res = "1111";
            break;
        default:
            break;
        }
        return res;
    }
    void loadProgram()
    {
        resetPointer();
        char opcode[7][4] = {"LDA", "STA", "ADD", "MUL", "DIV", "MOD", "SEA"};
        string s;
        while (getline(cin, s)) // !EOF
        {
            for (int i = 0; i < 3; i++)
                s[i] = toupper(s[i]); // upper

            for (int i = 0; i < 7; i++)
            {
                if (!s.substr(0, 3).compare(opcode[i]))
                {
                    mem[mpt] = getOpcode(i); // write opcode
                    string temp = hexToBinary(s.substr(4));
                    while (temp.size() < 12)
                        temp = "0" + temp;

                    mem[mpt++] += temp.substr(0, 4); // write operand
                    setMem(temp.substr(4));          // write operand
                    break;
                }
            }
        }
    };
    void setMem(int n)
    {
        // 현재 메모리주소에서 쓰기
        if (isVaildPointer())
            mem[mpt++] = decimalToBinary(n);
    };
    void setMem(string s)
    {
        // 현재 메모리주소에서 쓰기
        if (isVaildPointer())
            mem[mpt++] = s;
    };
    void setMem(int x, int n)
    {
        // 특정 주소 쓰기
        setPointer(x);
        if (isVaildPointer())
            mem[mpt++] = decimalToBinary(n);
    };
    void setMem(int x, string s)
    {
        // 특정 주소 쓰기
        setPointer(x);
        if (isVaildPointer())
            mem[mpt++] = s;
    };
    void setMemFirst(int n)
    {
        // 첫주소부터 쓰기
        setPointer(0);
        mem[mpt++] = decimalToBinary(n);
    };
    string getMem()
    {
        // 현재 메모리 주소에서 읽기
        if (isVaildPointer())
            return mem[mpt] + mem[mpt + 1];
    }
    string getMem(int x)
    {
        // 특정 주소부터 읽기
        setPointer(x);
        if (isVaildPointer())
            return mem[mpt] + mem[mpt + 1];
        else
            return decimalToBinary(0, 15);
    };
    string getMemFirst()
    {
        // 메모리 첫 주소에서부터 읽기
        setPointer(0);
        return mem[mpt++] + mem[mpt++];
    };
    int getMemHex()
    {
        return binaryToDecimal(getMem());
    };
    int getMemHex(int x)
    {
        return binaryToDecimal(getMem(x));
    }
    int getMemFirstHex()
    {
        return binaryToDecimal(getMemFirst());
    }

    void resetMem(int s, int e = MAXSIZE)
    {
        if (s < 0)
            s = 0;
        if (e > MAXSIZE)
            e = MAXSIZE;
        for (int i = s; i < e; i++)
        {
            mem[i] = "00000000";
        }
    }

    void resetMemAll()
    {
        resetMem(0);
    }

    void resetMemAll(int n)
    {
        if (n > 255)
            n = 255;
        string s = decimalToBinary(n);
        for (int i = 0; i < MAXSIZE; i++)
        {
            mem[i] = s;
        }
    }

    void resetMemAll(string s)
    {
        if (s.size() > 8)
            s = s.substr(0, 8);
        for (int i = 0; i < MAXSIZE; i++)
        {
            mem[i] = s;
        }
    }

    void resetPointer()
    {
        mpt = 0;
    }

    void setPointer(int n)
    {
        mpt = n;
    }

    bool isVaildPointer()
    {
        if (mpt >= 0 && mpt < MAXSIZE)
            return true;
        return false;
    }

    void showMem(int s, int e = MAXSIZE)
    {
        if (s < 0)
            s = 0;
        if (e > MAXSIZE)
            e = MAXSIZE;
        for (int i = s; i < e; i++)
        {
            cout << decimalToHex(binaryToDecimal(mem[i])) << endl;
        }
    };

    void showAllMem()
    {
        showMem(0);
    };

    void showMemBinary(int s, int e = MAXSIZE)
    {
        if (s < 0)
            s = 0;
        if (e > MAXSIZE)
            e = MAXSIZE;
        for (int i = s; i < e; i++)
        {
            cout << mem[i] << endl;
        }
    };

    void showAllMemBinary()
    {
        showMemBinary(0);
    };
};

class Register
{
protected:
    int data;

public:
    Register()
    {
        data = 0;
    };
    Register(int n)
    {
        data = n;
    };
    void setData(int n)
    {
        data = n;
    }
    int getData()
    {
        return this->data;
    }
    void clear()
    {
        data = 0;
    }
    void increment()
    {
        data++;
    }
};

class PC : public Register
{
private:
public:
    void next()
    {
        data += 2;
    }
    bool isValid()
    {
        if (data >= 0 && data < 4096)
            return true;
        return false;
    }
};

class IR : public Register
{
private:
public:
    int getOpcode()
    {
        return binaryToDecimal(decimalToBinary(data, 15).substr(0, 4));
    }
    int getOperand()
    {
        return binaryToDecimal(decimalToBinary(data, 15).substr(4));
    }
};

class AC : public Register
{
private:
    /* data */
public:
    bool isValid()
    {
        if (data >= 0 && data < 65535)
            return true;
        return false;
    }
};

class ALU
{
public:
    int add(int a, int b)
    {
        int res = a + b;
        if (res > 0xffff)   // overflow
            res %= 0x10000; // 뒷 4자리만
        return res;
    }
    int mul(int a, int b)
    {
        int res = a * b;
        if (res > 0xffff)   // overflow
            res %= 0x10000; // 뒷 4자리만
        return res;
    }
    int div(int a, int b)
    {
        if (b == 0 || a < b) // error
        {
            return 0;
        }

        return a / b;
    }
    int mod(int a, int b)
    {
        if (b == 0) // error
            return 0;
        return a % b;
    }
};

class CU
{

public:
    void excute(IR ir, AC &ac, MEM &mem, ALU alu)
    {

        switch (ir.getOpcode())
        {
        case 0:
            // LDA
            ac.setData(mem.getMemHex(ir.getOperand()));
            break;
        case 1:
            // STA
            mem.setMem(ir.getOperand(), decimalToBinary(ac.getData(), 15).substr(0, 8));
            mem.setMem(ir.getOperand() + 1, decimalToBinary(ac.getData(), 15).substr(8));
            break;
        case 2:
            // ADD
            ac.setData(alu.add(ac.getData(), ir.getOperand()));
            break;
        case 3:
            // MUL
            ac.setData(alu.mul(ac.getData(), ir.getOperand()));
            break;
        case 4:
            // DIV
            ac.setData(alu.div(ac.getData(), ir.getOperand()));
            break;
        case 5:
            // MOD
            ac.setData(alu.mod(ac.getData(), ir.getOperand()));
            break;
        case 15:
            // SEA
            ac.setData(ir.getOperand());
            break;

        default:
            cout << "invalid opcode!\n";
            break;
        }
    }
};

class CPU16
{
private:
    class ALU alu;
    class CU cu;
    class PC pc;
    class IR ir;
    class AC ac;

public:
    int getPc()
    {
        return pc.getData();
    }
    int getIr()
    {
        return ir.getData();
    }
    int getAc()
    {
        return ac.getData();
    }
    string readMem(MEM mem)
    {
        if (pc.isValid())
            return mem.getMem(pc.getData());
        cout << "pc is invalid\n";
        return decimalToBinary(0, 15);
    }
    int readMemHex(MEM mem)
    {
        if (pc.isValid())
            return mem.getMemHex(pc.getData());
        cout << "pc is invalid\n";
        return 0;
    }
    void fetch(MEM mem)
    {
        ir.setData(readMemHex(mem));
        pc.next();
    }
    void excute(MEM &mem)
    {
        cu.excute(ir, ac, mem, alu);
    }
};

int main()
{

    CPU16 *cpu = new CPU16();
    MEM *mem = new MEM();
    // mem->setMem(0x10, 0xff);
    // mem->setMem(0xff);
    mem->loadProgram();
    mem->showMemBinary(0, 10);
    for (int i = 0; i < 4; i++)
    {
        cout << "------------" << i + 1 << "------------------" << endl;

        cout << "AC: " << uppercase << hex << cpu->getAc() << endl;
        cout << "PC: " << uppercase << hex << cpu->getPc() << endl;
        cout << "IR: " << uppercase << hex << cpu->getIr() << endl;

        cpu->fetch(*mem);
        cpu->excute(*mem);
        cout << "AC: " << uppercase << hex << cpu->getAc() << endl;
        cout << "PC: " << uppercase << hex << cpu->getPc() << endl;
        cout << "IR: " << uppercase << hex << cpu->getIr() << endl;
        cout << "------------------------------" << endl;
    }
    mem->showMemBinary(0, 10);
    delete mem;
    delete cpu;
}
