#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
using namespace std;
unordered_map<string, string> optab;          // optab 해쉬테이블
unordered_map<string, pair<int, int>> symtab; // symtab 해쉬테이블
ifstream file;                                // 읽기용
ofstream wrfile;                              // 쓰기용
string start_adress;                          // 시작주소
int program_length;                           //프로그램 길이

string iToHexstring(int i)
{
    // intger to string(HEX)
    char tmp[10];
    sprintf(tmp, "%X", i);
    string ret = tmp;
    return ret;
}

void make_optab(char *OPTAB)
{ // optab 만드는 함수. 정적테이블이므로 미리 정의되있다.
    file.open(OPTAB);
    if (file.is_open())
    {
        while (!file.eof())
        {
            string Mnemonic, code;
            file >> Mnemonic >> code;
            while (Mnemonic.length() < 6)
            { // opcode의 길이를 6으로 맞쳐주기 위한 루프.
                Mnemonic.append(" ");
            }
            optab.insert({Mnemonic, code});
            // OPTAB에 삽입
        }
        cout << "optab is made" << endl;
    }
    else
    {
        cout << "error! optab is dosen't exit!\n";
        exit(0);
    }
    file.close();
}

int getByteLength(string oper)
{
    //문자열 길이 반환 ' ' 사이의 길이를 재 반환하는 원리
    // opcode가 byte일때 사용.
    int i = 0;
    while (oper[i++] != 39)
        ;
    int len = 0;
    while (oper[i++] != 39)
    {
        len++;
    }
    if (oper[0] == 'x' || oper[0] == 'X') // x'F1' << 이런 케이스
        len /= 2;
    return len;
}
int hexLength(int a)
{ // 16진수로서 길이가 어떻게 되는지 반환
    if (a == 0)
        return 1;
    int len = 0;
    while (a)
    {
        a /= 16;
        len++;
    }
    return len;
}

void write_symtab(char *SYMTAB)
{ // symbol table  출력
    wrfile.open(SYMTAB);
    wrfile << "SYMTAB" << endl;
    for (auto a : symtab)
    {
        wrfile << a.first;
        wrfile << "          ";
        wrfile << a.second.first << endl;
        if (a.second.second == 1)
        {
            // duplicate symbol
            wrfile << "duplicate symbol" << endl;
        }
    }
    cout << "sytmab is made" << endl;
    wrfile.close();
}
void make_symtab(int locctr, string label, bool &error)
{
    if (symtab.find(label) != symtab.end())
    {
        // set error flag (duplicate symbol)
        error = true;
        symtab[label].second = 1;
        cout << label << "is dublicate symbol!" << endl;
    }
    else
        symtab.insert({label, {locctr, 0}});
}

void write_start_end(string label, string opcode, string oper)
{
    //시작과 끝 중간파일 작성
    wrfile.write("              ", 14);
    wrfile << label;
    wrfile.write("OPCODE: ", 8);
    wrfile << opcode;
    wrfile.write("OPERAND: ", 9);
    wrfile << oper << endl;
}

void write_middle(string label, string opcode, string oper)
{
    wrfile.write("LABEL: ", 7);
    wrfile << label;
    wrfile.write("OPCODE: ", 8);
    wrfile << opcode;
    wrfile.write("OPERAND: ", 9);
    wrfile << oper;
    wrfile << endl;
}

void readline(string &str, string &label, string &opcode, string &oper)
{
    getline(file, str); //첫쨰 줄
    label = str.substr(0, 8);
    opcode = str.substr(9, 6);
    oper = str.substr(17);
    if (oper.find(' ') != string ::npos)
        oper = oper.erase(oper.find(' '));
}

int pass1(char *srcfile, char *intfile)
{
    file.open(srcfile);   // 읽을 파일 열기
    wrfile.open(intfile); // 쓸 파일 열기

    string label, opcode, oper;
    bool error = false;

    int locctr; // location counter
    if (file.is_open())
    {
        while (!file.eof()) // 파일 끝까지
        {
            string str;

            readline(str, label, opcode, oper);

            if (!opcode.compare("start ") || !opcode.compare("START "))
            { // opcode 가 start 이라면 operand를 시작주소로
                start_adress = oper;
                locctr = stoul(start_adress, nullptr, 16); // 16진수화
            }
            else
            {
                locctr = 0; // 0부터 시작
                start_adress = "000000";
            }

            write_start_end(label, opcode, oper); //첫째 줄 출력
            readline(str, label, opcode, oper);

            while (opcode.compare("end   ") && opcode.compare("END   "))
            {
                if (str[0] == '.')
                { // comment line skip{}
                    readline(str, label, opcode, oper);
                    continue;
                }

                if (label[0] != 32) // label이 있다면
                    make_symtab(locctr, label, error);

                //주소길이가 6자리보다 짧다면 앞에 0출력
                for (int i = 0; i < 6 - hexLength(locctr); i++)
                    wrfile << "0";

                wrfile << uppercase << hex << locctr << " "; // 주소 출력

                if (optab.find(opcode) != optab.end()) // optab에 있는 경우 ex)ldx,sta
                    locctr += 3;
                else if (!opcode.compare("word  ") || !opcode.compare("WORD  ")) // word 인 경우
                    locctr += 3;
                else if (!opcode.compare("resw  ") || !opcode.compare("RESW  ")) // resw인 경우
                    locctr += 3 * stoi(oper);
                else if (!opcode.compare("resb  ") || !opcode.compare("RESB  ")) // resb인 경우
                    locctr += stoi(oper);
                else if (!opcode.compare("byte  ") || !opcode.compare("BYTE  ")) // byte인 경우
                    locctr += getByteLength(oper);
                else
                {
                    // set error flag (invalid opcode)
                    error = true;
                    locctr += 3;
                    wrfile.write("invalid opcode -----------------------------\n", 15 + 30);
                    cout << opcode << " is invalid opcode" << endl;
                }

                write_middle(label, opcode, oper); // 가운데 중간파일 출력

                readline(str, label, opcode, oper);
            }
            if (label[0] != 32) // label이 있다면
                make_symtab(locctr, label, error);

            program_length = (locctr - stoul(start_adress, nullptr, 16)); // 프로그램 길이 저장
            break;
        }
        file.close(); // 파일 닫기

        write_start_end(label, opcode, oper); // 마지막 줄 출력
    }
    else
    {
        cout << "Srcfile file dosen't exit\n"; //파일 열지 못함
        return -1;
    }
    wrfile.close();
    if (error) // error 발생
        return -1;
    else
        return 0;
}

void writeHeader(string name)
{ // header레코드 작성
    wrfile.write("H", 1);
    wrfile.write(name.c_str(), 6); //프로그램 이름 출력
    while (start_adress.length() < 6)
        start_adress = '0' + start_adress; // 주소가 6자리보다 짧다면 '0'추가
    wrfile.write(start_adress.c_str(), 6); // 프로그램 시작주소 출력
    for (int i = 0; i < 6 - hexLength(program_length); i++)
        wrfile << "0";
    wrfile << hex << program_length; // 프로그램 길이 출력
    wrfile << endl;
}

int isOperDirect(string oper)
{ // direct인지 index인지 확인 , 가 있다면 ,인덱스반환 아니라면 길이 반환
    for (int i = 0; i < oper.length(); i++)
    {
        if (oper[i] == ',')
            return i;
    }
    return oper.length();
}

void readline2(string &str, string &start, string &label, string &opcode, string &oper)
{
    start = str.substr(0, 6);
    label = str.substr(14, 8);
    opcode = str.substr(30, 6);
    oper = str.substr(45);
}

int pass2(char *INTFILE, char *OBJFILE)
{
    file.open(INTFILE);
    wrfile.open(OBJFILE);
    bool error = false;

    if (file.is_open())
    {
        while (!file.eof())
        {
            string str, label, opcode, oper, start;

            getline(file, str); //첫쨰 줄
            readline2(str, start, label, opcode, oper);
            writeHeader(label);

            if (!opcode.compare("start ") || !opcode.compare("START "))
                getline(file, str);
            string textRecord = "T";
            string objectcode;

            while (opcode.compare("end   ") && opcode.compare("END   ")) // end가 아닐때까지
            {
                readline2(str, start, label, opcode, oper);

                if (textRecord.length() == 1 && opcode.compare("resb  ") && opcode.compare("resw  ") && opcode.compare("RESB  ") && opcode.compare("RESW  ")) // T레코드에 시작주소가 없다면
                    textRecord.append(start);

                if (str[0] != '.')
                { // not a comment line
                    if (optab.find(opcode) != optab.end())
                    { // search optabe for opcode

                        objectcode.append(optab[opcode]);

                        if (oper[0] != ' ' && !oper.empty())
                        { // serach symtab for operand
                            while (oper.length() < 8)
                                oper.append(" ");

                            if (symtab.find(oper) != symtab.end() || isOperDirect(oper) != oper.length())
                            { // symtab에 존재한다면 혹은 indirect 인 경우 Table,x
                                int address = 0;
                                if (isOperDirect(oper) != oper.length())
                                {
                                    address = 32768; // 0x8000
                                    oper = oper.substr(0, isOperDirect(oper));
                                    while (oper.length() < 8)
                                    {
                                        oper += " ";
                                    }
                                }
                                address += symtab[oper].first;
                                objectcode.append(iToHexstring(address));
                            }
                            else
                            {
                                // undefiend symbol error
                                error = true;
                                objectcode.append("0000");
                                cout << oper << "undefiend symbol error" << endl;
                            }
                        }
                        else
                        {
                            // stroe 0 as operand adress assemble the object code instruction ex)rsub
                            objectcode.append("0000");
                        }
                    }
                    else if (!opcode.compare("byte  ") || !opcode.compare("BYTE  "))
                    { // opcode가 byte 인 경우
                        if (oper[0] == 'x' || oper[0] == 'X')
                        {
                            int len = oper.length() - 3 + objectcode.length();
                            if (len >= 0x1e * 2) //길이가 1e를 넘어 새로운 레코드를 작성해야 하는 경우
                            {
                                int tmplen = objectcode.length();
                                textRecord.append("1E");
                                objectcode.append(oper.substr(2, 0x1e * 2 - tmplen));
                                wrfile << textRecord << objectcode << endl;                  // textrecord 작성
                                int next = stoul(textRecord.substr(1, 6), nullptr, 16) + 30; // 16진수화
                                string nextAddress = iToHexstring(next);
                                textRecord = "T";
                                for (int i = 0; i < 6 - nextAddress.length(); i++)
                                {
                                    textRecord.append("0");
                                }
                                objectcode.clear();
                                textRecord.append(nextAddress);
                                oper.erase(oper.end() - 1);
                                objectcode.append(oper.substr(2 + 0x1e * 2 - tmplen));
                            }
                            else
                            {
                                objectcode.append(oper.substr(2, oper.length() - 3));
                            }
                        }
                        else
                        { // c'eof'
                            int len = (oper.length() - 3) * 2 + objectcode.length();
                            char tmp[3];
                            if (len >= 0x1e * 2) //길이가 1e를 넘어 새로운 레코드를 작성해야 하는 경우
                            {
                                string total;
                                int tmplen = objectcode.length();
                                textRecord.append("1E");
                                wrfile << textRecord;

                                for (int i = 2; i < oper.length() - 1; i++)
                                {
                                    sprintf(tmp, "%X", (int)oper[i]);
                                    total.append(tmp);
                                }
                                objectcode.append(total.substr(0, 0x1e * 2 - objectcode.length()));

                                wrfile << objectcode << endl;
                                objectcode.clear();

                                int next = stoul(textRecord.substr(1, 6), nullptr, 16) + 30; // 16진수화
                                string nextAddress = iToHexstring(next);
                                textRecord = "T";
                                for (int i = 0; i < 6 - nextAddress.length(); i++)
                                {
                                    textRecord.append("0");
                                }

                                textRecord.append(nextAddress);
                                objectcode.append(total.substr(0x1e * 2 - tmplen));
                            }
                            else
                            {
                                for (int i = 2; i < oper.length() - 1; i++)
                                {
                                    sprintf(tmp, "%X", (int)oper[i]);
                                    objectcode.append(tmp);
                                }
                            }
                        }
                    }
                    else if (!opcode.compare("word  ") || !opcode.compare("WORD  "))
                    {
                        // word인경우
                        int temp = stoi(oper);
                        string word = iToHexstring(temp);
                        while (word.length() < 6)
                            word = '0' + word;

                        objectcode.append(word);
                    }
                    else
                    { // resw,resb인 경우
                        if (objectcode.length())
                        {
                            char tmp[3];
                            sprintf(tmp, "%X", objectcode.length() / 2);
                            if (objectcode.length() / 2 <= 16)
                                textRecord.append("0");
                            textRecord.append(tmp);                     // length 추가
                            wrfile << textRecord << objectcode << endl; // textrecord 작성
                            objectcode.clear();
                            textRecord = "T";
                        }
                    }
                }

                if (objectcode.length() >= 0x1e * 2)
                {                                                             //길이가 1e가 넘으면 새 레코드 생성
                    textRecord.append(iToHexstring(objectcode.length() / 2)); // length 추가
                    wrfile << textRecord << objectcode << endl;               // textrecord 작성
                    objectcode.clear();
                    textRecord = "T";
                }
                getline(file, str);
            }

            if (objectcode.length())
            {                                                             //목적코드가 존재한다면
                textRecord.append(iToHexstring(objectcode.length() / 2)); // length 추가
                wrfile << textRecord << objectcode << endl;
            }

            wrfile << "E" << start_adress; // End ecord
        }
    }
    else
    {
        cout << "INTFILE file dosen't exit\n"; //파일을 열지 못함
        return -1;
    }
    file.close();
    wrfile.close();
    if (error)
        return -1;
    return 0;
}

int main(int argc, char **argv)
{

    if (argc != 6)
    {
        cout << "input : OPTAB SRCFILE INTFILE SYMTAB OBJFILE PLEASE!\n";
        // PATH,OPTAB,SRCFILE,INTFILE,SYMTAB,OBJFILE
        return 0;
    }
    make_optab(argv[1]);
    if (pass1(argv[2], argv[3]) == -1)
    {
        cout << "pass1 error! program exit...\n";
        return 0;
    }
    else
        cout << "pass1 is OK\n";

    write_symtab(argv[4]); // symboltable 출력
    if (pass2(argv[3], argv[5]) == -1)
    {
        cout << "pass2 error! program exit...\n";
    }
    else
        cout << "pass2 is OK\nAssemble Complete!!!";

    return 0;
}