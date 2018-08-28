// ConsoleApplication1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <jni.h>
#include <win32/jni_md.h>
#include <Windows.h>
std::wstring get_env(LPCWSTR s)
{
	std::wstring appdata(1024, ' ');

	DWORD len = GetEnvironmentVariableW(s, &appdata[0], appdata.size());
	appdata.resize(len);
	return appdata;
}
char* replace_mainclass(char* text)
{
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
	{
		if(text[i] == '.') text[i] = '/';
	}
	return text;
}
int main( int argc,char** argv)
{
	std::cerr << "Load JVM\n";
	JNIEnv* env = nullptr;
	JavaVM* jvm = nullptr;
	JavaVMInitArgs vm_args;
	int jvm_args = 0;
	JavaVMOption* options = new JavaVMOption[argc];
	for (int i = 0; i < argc; ++i)
	{
		if (argv[i][0] != '-' && i!=0) {
			jvm_args = i;
			break;
		}
		std::cout << argv[i] << std::endl;
		options[i].optionString = argv[i];
	}
	vm_args.version = JNI_VERSION_1_8;
	vm_args.nOptions = jvm_args;
	vm_args.options = options;
	vm_args.ignoreUnrecognized = JNI_TRUE; // remove unrecognized options
	std::wstring jvmdll_path =  get_env(L"JAVA_HOME") + TEXT("\\bin\\server\\jvm.dll"); //May be JDK
	std::wstring jvmdll_path2 = get_env(L"JAVA_HOME") + TEXT("\\jre\\bin\\server\\jvm.dll"); //May be JRE
	std::wstring jvmdll_path3 = get_env(L"JAVA_HOME") + TEXT("\\jre\\bin\\client\\jvm.dll"); //May be JDK
	std::wstring jvmdll_path4 = get_env(L"JAVA_HOME") + TEXT("\\bin\\client\\jvm.dll"); //May be JRE
	HINSTANCE hinstLib = LoadLibrary(jvmdll_path.c_str());
	if(hinstLib == NULL) hinstLib = LoadLibrary(jvmdll_path2.c_str());
	if(hinstLib == NULL) hinstLib = LoadLibrary(jvmdll_path3.c_str());
	if(hinstLib == NULL) hinstLib = LoadLibrary(jvmdll_path4.c_str());
	typedef jint(JNICALL *PtrCreateJavaVM)(JavaVM **, void **, void *);
	PtrCreateJavaVM ptrCreateJavaVM = (PtrCreateJavaVM)GetProcAddress(hinstLib, "JNI_CreateJavaVM");
	int ret = ptrCreateJavaVM(&jvm, (void**)&env, &vm_args);
	if(ret == JNI_ERR) std::cout << "Error creating JVM\n";
	else if (ret == JNI_OK) std::cout << "JVM created OK\n";
	char* mainclass = replace_mainclass(argv[jvm_args]);
	std::cout << "mainclass:" << mainclass << std::endl;
	jclass MainClass = env->FindClass(mainclass);
	jmethodID MainMethod = env->GetStaticMethodID(MainClass, "main", "([Ljava/lang/String;)V");
	jclass StringClass = env->FindClass("java/lang/String");
	int numOfArguments = argc - jvm_args - 1;
	int argumentIndex = 0;

	jobjectArray jargs = env->NewObjectArray(numOfArguments, StringClass, NULL);
	for (int i = jvm_args + 1; i < argc; ++i)
	{
		env->SetObjectArrayElement(jargs, i - jvm_args - 1, env->NewStringUTF(argv[i]));
		std::cout << argv[i] << std::endl;
	}
	env->CallStaticObjectMethod(MainClass, MainMethod,jargs);
    std::cout << "JVM stopped\n";
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
