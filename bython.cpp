#include <bits/stdc++.h>
#include <cstddef>
#include <cstring>

using namespace std;

// show every line that executes
bool DEBUG = 0;
// show memory at the end
bool MEMORY_MAP = 0;

string code =
    // Variable names (important)
    // The first row of the keyboard is used as arguments (and output) to "functions": qwertyuiop
    // The second row is used by "functions": asdfghjkl
    // The third row is for the program: zxcvbnm
    // "function" is a procedure that is working with data from first row
    // Good luck!

    // Absolute value of `q` .
    // input: q (int or float)
    // temp values: a
    // output: q (int or float)
    "proc std_abs;"
        "create int a;"
        "gre 0 q a;"
        "if a;"
            "sub 0 q q;"
        "ret;"
    "endproc;"

    // Modulo of `q` divided by `w`.
    // Works only if w < q.
    // Warning: will cause stack overflow if (w * 16384) > q.
    // input: q, w (int or float)
    // temp values: a
    // output: q (int or float)
    // output: q = -1 if w is 0
    "proc std_modulo;"
        "create int a;"
        "gre w q a;"
        "if a;"
            "ret;"
        "sub q w q;"
        "exec std_modulo;"
        "ret;"
    "endproc;"

    // Your code
    "print Hello, world!;"
;

const int bsize = 5;
unsigned char memory[1000];
map<string, int> procedures;
int stk[16384];
int stkp = 0;
bool skip = false;
bool procdef = false;
bool holdon = false;
int ori = 0;

void store_float(float val, int addr) {
    memory[addr] = 'f';
    memcpy(&memory[addr+1], &val, (size_t)(4));
}

void store_int(int val, int addr) {
    memory[addr] = 'i';

    for (int i = 3; i >= 0; i--) {
        memory[addr + i + 1] = val % 256;
        val >>= 8;
    }
}

int get_int(int addr) {
    int val = 0;

    if (memory[addr] != 'i') {
        cout << "Tried to get int but other type found";
        exit(1);
    }

    for (int i = 0; i < 4; i++) {
        val <<= 8;
        val += memory[addr + i + 1];
    }

    return val;
}

float get_float(int addr) {
    float val;

    if (memory[addr] != 'f') {
        cout << "Tried to get float but other type found";
        exit(1);
    }
    
    memcpy(&val, &memory[addr+1], (size_t)(4));

    return val;
}

int parse_int_arg(string argv) {
    int val;

    if ((argv[0] < '0' || argv[0] > '9') && argv[0] != '-') {
        val = get_int(argv[0] * bsize);
    } else {
        val = atoi(argv.c_str());
    }

    return val;
}

float parse_float_arg(string argv) {
    float val;

    if (argv[0] >= 'a' && argv[0] <= 'z') {
        val = get_float(argv[0] * bsize);
    } else {
        val = atof(argv.c_str());
    }

    return val;
}

void b_proc(int index, int argc, vector<string> &argv) {
    if (argc != 1) {
        cout << "Usage of proc:\n- proc procedurename";
        exit(1);
    }

    procedures[argv[0]] = index;
    procdef = true;
}

void b_exec(int argc, vector<string> &argv) {
    if (argc != 1) {
        cout << "Usage of exec:\n- exec shmemoryslot";
        exit(1);
    }

    holdon = true;
    skip = true;
    stk[stkp] = ori;
    stkp++;
    ori = procedures[argv[0]];
}

void b_ret(int argc, vector<string> &argv) {
    skip = true;
    holdon = true;
    stkp--;
    ori = stk[stkp];
}

void b_input(int argc, vector<string> &argv) {
    if (argc != 1) {
        cout << "Usage of input:\n- input memoryslot";
        exit(1);
    }

    int addr = argv[0][0] * bsize;

    if (memory[addr] == 'i') {
        int inp;
        cin >> inp;
        store_int(inp, addr);
        
        return;
    }

    if (memory[addr] == 'f') {
        float inp;
        cin >> inp;
        store_float(inp, addr);
        
        return;
    }

    cout << "Unknown variable type";
    exit(1);
}

void b_if(int argc, vector<string> &argv) {
    if (argc != 1) {
        cout << "Usage of if:\n- if int/memoryslot";
        exit(1);
    }

    int val = parse_int_arg(argv[0]);

    if (val == 0) {
        skip = true;
    }
}

void b_rshi(int argc, vector<string> &argv) {
    if (argc != 2) {
        cout << "Usage of rshi:\n- rshi memoryslot shiftamount";
        exit(1);
    }

    int addr = argv[0][0] * bsize;

    if (memory[addr] != 'i') {
        cout << "rshi only works with int";
        exit(1);
    }

    int shift = atoi(argv[1].c_str());

    int val = get_int(addr);
    val >>= shift;

    store_int(val, addr);
}

void b_print(int argc, vector<string> &argv) {
    if (argc < 1) {
        cout << "Usage of print:\n- print memoryslot/string";
        exit(1);
    }

    if (argv[0].length() == 1 && argv[0][0] >= 'a' && argv[0][0] <= 'z') {
        int addr = argv[0][0] * bsize;

        if (memory[addr] == 'i') {
            cout << get_int(addr);
        }
        if (memory[addr] == 'f') {
            cout << get_float(addr);
        }
    } else {
        for (int i = 0; i < argc; i++) {
            cout << argv[i];
            if (i + 1 != argc) cout << " ";
        }
    }
}

void b_gre(int argc, vector<string> &argv) {
    if (argc != 3) {
        cout << "Usage of gre:\n- gre num1 num2 memoryslot";
        exit(1);
    }

    int store_addr = argv[2][0] * bsize;
    char t1, t2;

    if (argv[0].length() == 1 && argv[0][0] >= 'a' && argv[0][0] <= 'z') {
        t1 = memory[argv[0][0] * bsize];
    } else {
        if (argv[0].find(".") == -1) {
            t1 = 'i';
        } else {
            t1 = 'f';
        }
    }

    if (argv[1].length() == 1 && argv[1][0] >= 'a' && argv[1][0] <= 'z') {
        t2 = memory[argv[1][0] * bsize];
    } else {
        if (argv[1].find(".") == -1) {
            t2 = 'i';
        } else {
            t2 = 'f';
        }
    }

    if (t1 == t2 && t1 == 'i') {
        int a = parse_int_arg(argv[0]);
        int b = parse_int_arg(argv[1]);
        int result = a > b;

        store_int(result, store_addr);
    } else {
        float a = parse_float_arg(argv[0]);
        float b = parse_float_arg(argv[1]);
        float result = a > b;

        store_int(result, store_addr);
    }
}

void b_add(int argc, vector<string> &argv, bool sub = false) {
    if (argc != 3) {
        cout << "Usage of add:\n- add num1 num2 memoryslot";
        exit(1);
    }

    int store_addr = argv[2][0] * bsize;
    char t1, t2;

    if (argv[0].length() == 1 && argv[0][0] >= 'a' && argv[0][0] <= 'z') {
        t1 = memory[argv[0][0] * bsize];
    } else {
        if (argv[0].find(".") == -1) {
            t1 = 'i';
        } else {
            t1 = 'f';
        }
    }

    if (argv[1].length() == 1 && argv[1][0] >= 'a' && argv[1][0] <= 'z') {
        t2 = memory[argv[1][0] * bsize];
    } else {
        if (argv[1].find(".") == -1) {
            t2 = 'i';
        } else {
            t2 = 'f';
        }
    }

    if ((t1 == 'i') && (t2 == 'i')) {
        int a = parse_int_arg(argv[0]);
        int b = parse_int_arg(argv[1]) * -(sub * 2 - 1);
        int result = a + b;

        store_int(result, store_addr);
    } else {
        float a = parse_float_arg(argv[0]);
        float b = parse_float_arg(argv[1]) * -(sub * 2 - 1);
        float result = a + b;

        store_float(result, store_addr);
    }
}

void b_sub(int argc, vector<string> &argv) {
    b_add(argc, argv, true);
}

void b_create(int argc, vector<string> &argv) {
    string type = argv[0];
    
    if (type == "int") {
        if (argc > 3) {
            cout << "Too much arguments for creating an int";
            exit(1);
        }

        int value = 0;
        if (argc == 3) {
            value = atoi(argv[2].c_str());
        }

        if (argv[1].length() != 1) {
            cout << "Variable name should be a single char";
            exit(1);
        }

        store_int(value, argv[1][0] * bsize);
    }
    if (type == "float") {
        if (argc > 3) {
            cout << "Too much arguments for creating an int";
            exit(1);
        }

        float value = .0;
        if (argc == 3) {
            value = atof(argv[2].c_str());
        }

        if (argv[1].length() != 1) {
            cout << "Variable name should be a single char";
            exit(1);
        }

        store_float(value, argv[1][0] * bsize);
    }
}

void interpretate(string &line, int length, int orin) {
    if (length == 0) {
        return;
    }

    int first_space = line.find(" ");
    string command = line.substr(0, first_space);

    int argc = 0;
    vector<string> argv;
    int index = first_space + 1;
    while (index < length) {
        int space = line.find(" ", index);

        if (space == -1) {
            space = length;
        }

        argc++;
        argv.push_back(line.substr(index, space - index));

        index = space + 1;
    }

    if (command == "proc") return b_proc(orin, argc, argv);
    if (command == "endproc") { procdef = false; return; }

    if (procdef) return;

    if (DEBUG)
        cout << line << endl;

    if (command == "ret") return b_ret(argc, argv);
    if (command == "exec") return b_exec(argc, argv);

    if (command == "if") return b_if(argc, argv);

    if (command == "create") return b_create(argc, argv);
    if (command == "add") return b_add(argc, argv);
    if (command == "sub") return b_sub(argc, argv);
    if (command == "gre") return b_gre(argc, argv);
    if (command == "print") return b_print(argc, argv);
    if (command == "input") return b_input(argc, argv);
    if (command == "rshi") return b_rshi(argc, argv);

    cout << "Unknown command: " << command;
    exit(1);
}

void bython(string &code) {
    int length = code.length();

    while (ori < length) {
        int newnl = code.find(';', ori);

        if (newnl == -1) newnl = length;

        int line_length = newnl - ori;

        if (!skip) {
            string line = code.substr(ori, line_length);
            interpretate(line, line_length, ori);
        } else {
            skip = false;
        }

        if (!holdon)
            ori = newnl + 1;

        holdon = false;
    };
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);

    bython(code);

    if (MEMORY_MAP) {
        cout << "MEMORY\n";
        for (int i = 0; i < 1000 / 20; i++) {
            for (int j = 0; j < 20; j++)
                cout << (int)(memory[i*20+j]) << " ";
            cout << endl;
        }
    }
}
