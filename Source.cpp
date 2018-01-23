#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <time.h>
#include <conio.h>
#include <cstdio>
#include <Windows.h>
#include <iomanip>


using namespace std;
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
time_t now;
struct tm currentime;
const int maxLength = 200;
unsigned int historyLength = 0;
unsigned int length = 0;

enum TransactionStatus {
	SUCCESS = 1,
	NO_BALANCE = 2,
	WRONG_RECEIVER_ID = 3,
	_ERROR = 4
};

struct Hesab {
	string ID;
	string PIN;
	double Balance;
};

struct Transaction {
	string payerId;
	string receiverId;
	double amount;
	TransactionStatus status;
	struct tm time;
	time_t weekDay;
};

Hesab Account[maxLength];
Transaction History[200];

void Back_to_Menu();
void loadFromFile();
void saveToFile();
void loadFromHistoryFile();
void saveToHistoryFile();
string Get_Random_Number(int length);
string Create_Your_PIN();
Hesab Increase_Your_Balance(Hesab *arr, int i);
Hesab Decrease_Your_Balance(Hesab *arr, int i);
Hesab Operattions_On_Account(Hesab *arr);
void Create_New_Account();
void Money_Transfer();
void Check_Your_Balance();
void Display_History();
void displaySuccessTransactions(string accountID);
void displayWrongTransactions(string accountID);
void timeOut();

// Bu funksiya konsolda kursorun görünməməsini təmin edir.
void hideCursor(bool is_visible = false) {
	CONSOLE_CURSOR_INFO info;
	info.bVisible = is_visible;
	info.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

int main() {
	loadFromFile();
	loadFromHistoryFile();
	hideCursor();
	string Menu[6] = { "CREATE NEW ACCOUNT", "OPERATIONS ON ACCOUNT", "CHECK YOUR BALANCE", "MONEY TRANSFER", "HISTORY", "EXIT" };
	int pointer = 0, key = 0;
	while (true) {
		SetConsoleCursorPosition(h, { 0, 2 });
		SetConsoleTextAttribute(h, 10);
		cout << " ******************************************************" << endl;
		cout << " *                    MAIN MENU                       *" << endl;
		cout << " ******************************************************" << endl << endl;
		for (int i = 0; i < 6; i++) {
			if (i == pointer) {
				SetConsoleTextAttribute(h, 139);
				cout << "---> " << Menu[i] << endl;
			}
			else {
				SetConsoleTextAttribute(h, 11);
				cout << "     " << Menu[i] << endl;
			}
		}
		Sleep(100);
		key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) {
				pointer--;
				if (pointer == -1) pointer = 5;
			}
			else if (key == 80) {
				pointer++;
				if (pointer == 6) pointer = 0;
			}
		}
		else if (key == 13) {
			switch (pointer) {
			case 0: {
				system("cls");
				Create_New_Account();
				Back_to_Menu();
				break;
			}
			case 1: {
				Operattions_On_Account(Account);
				break;
			}
			case 2: {
				Check_Your_Balance();
				Back_to_Menu();
				break;
			}
			case 3: {
				Money_Transfer();
				Back_to_Menu();
				break;
			}
			case 4: {
				system("cls");
				Display_History();
				system("cls");
				break;
			}
			case 5: {
				saveToHistoryFile();
				saveToFile();
				exit(0);
			}
			}
		}
	}
	return 0;
}

/*
Bu funksiya programın bir neçə digər funksiyaları işini bitirdikdən sonra icra olunur və "BACK TO MAIN MENU"
və "EXIT" seçimlərindən ibarət olub, uyğun olaraq başlanğıc menyuya qayıtmaq və programın işinin dayandırılması işini icra edir.
Funksiyadakı "key" dəyişəni USER-in UP & DOWN klavişlərinə basması ilə "pointer" dəyişənin increment və ya decrement olunub,
string tipli "menu" massivinin uyğun elementinin fərqli rəngdə ekranda görünməsini təmin edir. USER "ENTER" klavişini sıxdıqda isə
program pointer dəyişənin aldığı son dəyərə əsasən "switch (pointer)" dövrünə girib müvafiq funksiyaları icra edir.
*/

void Back_to_Menu() {
	int pointer = 0, key = 0;
	string menu[2] = { "BACK TO MAIN MENU", "EXIT" };
	while (true) {
		SetConsoleCursorPosition(h, { 0, 12 });
		for (int i = 0; i < 2; i++) {
			if (i == pointer) {
				SetConsoleTextAttribute(h, 139);
				cout << "--> " << menu[i] << endl;
			}
			else {
				SetConsoleTextAttribute(h, 11);
				cout << "    " << menu[i] << endl;
			}
		}
		key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) {
				pointer--;
				if (pointer == -1) pointer = 1;
			}
			else if (key == 80) {
				pointer++;
				if (pointer == 2) pointer = 0;
			}
		}
		else if (key == 13) {
			switch (pointer) {
			case 0: {
				system("cls");
				saveToHistoryFile();
				saveToFile();
				return;
			}
			case 1: {
				saveToHistoryFile();
				saveToFile();
				exit(0);
			}
			}
		}
	}
}

/*
Bu funksiya "length" sayda təsadüfi ədədləri string tipli "random_number" dəyişəninə mənimsədib
onu eyni tipdə qaytarır. Funksiya parametr olaraq int tipli ədəd alıb uyğun sayda simvol emal edir
*/

string Get_Random_Number(int length) {
	srand(time(0));
	string random_number = "";
	for (int i = 0; i < length; i++)
		random_number += to_string(rand() % 10);
	return random_number;
}

/*
Bu funksiya char tipində "pin" adlı massivə yalnız rəqəm olmaq şərtilə 4 simvolu mənimsədib eyni tipdə geri qaytarır.
İstifadəçi düzgün simvollar daxil etdikdə bool tipli "onlyNumbers" dəyişəni true olduğu üçün dövrün şərti (!onlyNumbers)
ödənmir və dövr bitir, əks halda isə dövr davam edir.
*/

string Create_Your_PIN() {
	char pin[5];
	bool onlyNumbers = false;
	system("cls");
	do {
		SetConsoleCursorPosition(h, { 2, 3 }); SetConsoleTextAttribute(h, 11);
		cout << "\n [Type Your PIN] : ";
		for (int i = 0; i < 4; i++) {
			pin[i] = _getch();
			if (pin[i] >= 48 && pin[i] <= 57) { onlyNumbers = true; }
			else {
				onlyNumbers = false;
				SetConsoleCursorPosition(h, { 1, 0 }); SetConsoleTextAttribute(h, 12);
				system("cls");
				cout << " ***************************************************" << endl;
				cout << " *          PLEASE TYPE ONLY NUMBERS!!!            *" << endl;
				cout << " ***************************************************" << endl;
				break;
			}
			cout << '*';
		}
	} while (!onlyNumbers);
	pin[4] = '\0';
	return pin;
}

/*
Bu funksiya Hesab tipli "Account" struct-ı ilə bağlı aparılan bütün dəyişiklikləri
"Accounts.txt" adli fayla yazmaq işini icra edir.
*/
void saveToFile() {
	ofstream fout("Accounts.txt");
	for (int i = 0; i < length; i++) {
		fout << Account[i].ID << ' ' << Account[i].PIN << ' ' << Account[i].Balance << endl;
	}
	fout.close();
}

/*
Bu funksiya Hesab tipli "Account" struct-ı ilə bağlı aparılan bütün dəyişiklikləri
"Accounts.txt" adli fayldan programa oxumaq işini icra edir.
*/
void loadFromFile() {
	ifstream fin("Accounts.txt");
	if (fin.is_open()) {
		while (!fin.eof()) {
			fin >> Account[length].ID >> Account[length].PIN >> Account[length].Balance;
			if (!fin.eof()) {
				length++;
			}
		}
		fin.close();
	}
}

/*
Bu funksiya Transaction tipli "History" struct-ı ilə bağlı aparılan bütün dəyişiklikləri
"History.txt" adli fayla yazmaq işini icra edir.
*/
void saveToHistoryFile() {
	ofstream fout("History.txt");
	for (int i = 0; i < historyLength; i++) {
		fout << History[i].payerId << ' ' << History[i].receiverId << ' ' << History[i].amount
			<< ' ' << History[i].status << ' ' << History[i].time.tm_mday << ' ' << History[i].time.tm_mon
			<< ' ' << History[i].time.tm_year << ' ' << History[i].weekDay << endl;
	}
	fout.close();
}

/*
Bu funksiya Transaction tipli "History" struct-ı ilə bağlı aparılan bütün dəyişiklikləri
"History.txt" adli fayldan programa oxumaq işini icra edir.
*/
void loadFromHistoryFile() {
	ifstream fin("History.txt");

	if (fin.is_open()) {
		short enumtype;
		while (!fin.eof()) {
			fin >> History[historyLength].payerId >> History[historyLength].receiverId
				>> History[historyLength].amount >> enumtype;
			History[historyLength].status = (TransactionStatus)enumtype;
			fin >> History[historyLength].time.tm_mday >> History[historyLength].time.tm_mon
				>> History[historyLength].time.tm_year >> History[historyLength].weekDay;
			if (!fin.eof()) {
				historyLength++;
			}
		}
		fin.close();
	}
}

/* Bu funksiya ilk olaraq string tipli "temp_number" dəyişəninə Get_Random_Number()
funksiyasının qaytardığı 6 simvolu mənimsədib, onun Account struct-nın ID adlı digər
elementləri ilə eyni olub-olmadığını yoxlayır. Təkrar olduğu halda bool tipli "duplicate"
dəyişəni true olduğu üçün dövr davam edir. Əks halda "temp_number" dəyişənin dəyəri struct-ın
növbəti ID elementinə mənimsədilir və struct-ın Balance elementi sıfırlanır. Daha sonra menu seçimi
vasitəsilə (Back_To_Menu() funksiyasında olduğu kimi) USER PIN-in özünün ( Create_Your_PIN() funksiya ilə)
və ya random olaraq ( Get_Random_Number() funksiyası ilə ) yaradılması seçimini edir. Sonda yaradılan yeni
hesabın məlumatları (İD, PİN, BALANCE) ekrana çap olunur.
*/
void Create_New_Account() {
	bool duplicate = false;
	string temp_number;
	do {
		temp_number = Get_Random_Number(6);
		for (int i = 0; i < length; i++) {
			if (temp_number == Account[i].ID)
				duplicate = true;
		}
	} while (duplicate == true);
	Account[length].ID = temp_number;
	Account[length].Balance = 0.0;
	length++;
	string menu[2] = { "YES","RANDOM" };
	short pointer = 0, key = 0;
	SetConsoleCursorPosition(h, { 0, 1 });
	SetConsoleTextAttribute(h, 10);
	cout << " ***************************************************" << endl;
	cout << " *              CREATE NEW ACCOUNT                 *" << endl;
	cout << " ***************************************************" << endl << endl;
	while (true) {
		SetConsoleCursorPosition(h, { 0, 5 });
		SetConsoleTextAttribute(h, 11);
		cout << " WOULD YOU WANT TO CREATE YOUR PIN YOURSELF?\n" << endl;
		for (int i = 0; i < 2; i++) {
			if (i == pointer) {
				SetConsoleTextAttribute(h, 139);
				cout << "---> " << menu[i] << endl;
			}
			else {
				SetConsoleTextAttribute(h, 11);
				cout << "     " << menu[i] << endl;
			}
		}
		key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) {
				pointer--;
				if (pointer == -1)
					pointer = 1;
			}
			else if (key == 80) {
				pointer++;
				if (pointer == 2)
					pointer = 0;
			}
		}
		else if (key == 13) {
			switch (pointer) {
			case 0: {
				SetConsoleTextAttribute(h, 11);
				Account[length - 1].PIN = Create_Your_PIN();
				break;
			}
			case 1: {
				SetConsoleTextAttribute(h, 11);
				Account[length - 1].PIN = Get_Random_Number(4);
				break;
			}
			}
			break;
		}
	}
	system("cls");
	SetConsoleTextAttribute(h, 10);
	cout << " ***************************************************" << endl;
	cout << " *              CONGRATULATIONS!!!                 *" << endl;
	cout << " *           ACCOUNT HAS BEEN CREATED              *" << endl;
	cout << " ***************************************************" << endl << endl;
	cout << "\n  Your Account ID : " << Account[length - 1].ID << endl;
	cout << "  Your PIN        : " << Account[length - 1].PIN << endl;
	printf("  Your Balance    : %.2f AZN\n\n", Account[length - 1].Balance);
	return;
}

/*
Bu funksiya Hesab tipli "*arr" göstəricisini (pointerini) və int tipli "i" dəyişənini parametr
olaraq qəbul edir. USER tərəfindən girilən məbləğ 0-dan böyük olduğu halda *arr pointerinin
göstərdiyi "Account" struct-nın i saylı ID-nin "Balance" elementini girilən double tipli "money"
dəyişəni qədər artırıb, onu cari vaxtla birgə ekrana çap edib, Account strukturunu geri qaytarır.
Məbləğ 0-dan kiçik olduğu halda heç bir əməliyyat yerinə yetirilmədən struktur geri qaytarılır.
*/
Hesab Increase_Your_Balance(Hesab *arr, int i) {
	system("cls");
	now = time(NULL);
	currentime = *localtime(&now);
	double money = 0.0;
	SetConsoleCursorPosition(h, { 0, 1 });
	SetConsoleTextAttribute(h, 10);
	cout << " ***************************************************" << endl;
	cout << " *             INCREASE YOUR BALANCE               *" << endl;
	cout << " ***************************************************" << endl;
	SetConsoleTextAttribute(h, 11);
	cout << " [ENTER AMOUNT] : "; cin >> money;
	if (money > 0) {
		arr[i].Balance += money;
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 10);
		cout << " ***************************************************" << endl;
		cout << " *       TRANSACTION COMPLETED SUCCESSFULLY        *" << endl;
		cout << " ***************************************************" << endl << endl;
		cout << " YOUR BALANCE : " << setprecision(2) << fixed << arr[i].Balance << " AZN" << endl << endl;
		SetConsoleTextAttribute(h, 15);
		cout << " DATE : " << asctime(localtime(&now));
		return *arr;
	}
	else {
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 12);
		cout << " *************************************" << endl;
		cout << " *        WRONG AMOUNT!!!            *" << endl;
		cout << " *       PLEASE TRY AGAIN            *" << endl;
		cout << " *************************************" << endl;
		return *arr;
	}
}

/*
Bu funksiya Hesab tipli "*arr" göstəricisini (pointerini) və int tipli "i" dəyişənini
parametr olaraq qəbul edir. USER tərəfindən girilən məbləğ 0-dan böyük olduğu və Balance-daki
mövcud dəyərdən kiçik-bərabər olduğu halda *arr pointerinin göstərdiyi "Account" struct-nın
i saylı ID-nin "Balance" elementini girilən double tipli "money" dəyişəni qədər azaldıb,
onu ekrana cari vaxtla birgə çap edib, Account strukturunu geri qaytarır. Məbləğ 0-dan kiçik olduğu
və ya Balance-daki mövcud dəyərdən böyük olduğu halda heç bir əməliyyat yerinə yetirilmədən struktur geri qaytarılır.
*/
Hesab Decrease_Your_Balance(Hesab *arr, int i) {
	system("cls");
	now = time(NULL);
	currentime = *localtime(&now);
	double money = 0.0;
	SetConsoleCursorPosition(h, { 0, 1 });
	SetConsoleTextAttribute(h, 10);
	cout << " ***************************************************" << endl;
	cout << " *      WITHDRAW MONEY FROM YOUR BALANCE           *" << endl;
	cout << " ***************************************************" << endl;
	SetConsoleTextAttribute(h, 11);
	cout << " [ENTER AMOUNT] : "; cin >> money;
	if (money <= 0) {
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 12);
		cout << " *************************************" << endl;
		cout << " *        WRONG AMOUNT!!!            *" << endl;
		cout << " *       PLEASE TRY AGAIN            *" << endl;
		cout << " *************************************" << endl;
		return *arr;
	}
	else if (money > arr[i].Balance) {
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 12);
		cout << " *************************************" << endl;
		cout << " * NO ENOUGH FUNDS ON YOUR ACCOUNT!  *" << endl;
		cout << " *   PLEASE INCREASE YOUR BALANCE    *" << endl;
		cout << " *************************************" << endl;
		cout << " \nYOUR BALANCE : " << setprecision(2) << fixed << arr[i].Balance << " AZN" << endl << endl;
		return *arr;
	}
	else if (money <= arr[i].Balance) {
		system("cls");
		arr[i].Balance -= money;
		SetConsoleCursorPosition(h, { 0, 4 });
		SetConsoleTextAttribute(h, 6);
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 10);
		cout << " ***************************************************" << endl;
		cout << " *       TRANSACTION COMPLETED SUCCESSFULLY        *" << endl;
		cout << " ***************************************************" << endl << endl;
		cout << " YOUR BALANCE : " << setprecision(2) << fixed << arr[i].Balance << " AZN" << endl << endl;
		SetConsoleTextAttribute(h, 15);
		cout << " DATE : " << asctime(localtime(&now));
		return *arr;
	}
}

/*
Bu funksiya USER-dən string tipli account_ID dəyişəninə dəyər alıb onu Account struct-da axtarıb
tapdıqdan sonar string tipli "pin" dəyişəninə char tipli "_pin[4]" massivinin simvollarını toplayıb
uyğun Account-un elementinin PİN-i ilə yoxlayır. _pin[4] massivinin hər simvolu getch() funksiyası ilə
girildikdə ekrana "*" simvolu çap olunur. Girilənlər doğru olduğu halda bool tipli found dəyişəni true
olur və program dövrə girərək istifadəçiyə menyudan seçim etməsini gözləyir.
UP & DOWN (Back_To_Menu () funksiyasındakı kimi) klavişləri ilə USER menyuda seçim edir və case-lərə uyğun
olaraq Increase_Your_Balance() və Decrease_Your_Balance() funksiyaları icra olunur. 3-cu menu sətri heç
bir iş gormədən funksiyanı sonlandırır, 4-cü isə programdan çıxışı həyata keçirir. wrongPinCount dəyişəni
yanlış girilmiş PİN-ləri sayıb 3 qiymətini aldıqda timeOut () funksiyasını icra olunur. Qiymət 6-ya bərabər
olduqda isə funksiya sonlanır.
*/
Hesab Operattions_On_Account(Hesab *arr) {
	string menu[4] = { "DEPOSIT MONEY", "WITHDRAW MONEY", "BACK TO MENU", "EXIT" };
	string account_ID, pin;
	short wrongPinCount = 0;
	char _pin[4];
	int pointer = 0, key = 0;
	bool found = false;
	system("cls");
	SetConsoleCursorPosition(h, { 0, 1 });
	SetConsoleTextAttribute(h, 10);
	cout << " ******************************************************" << endl;
	cout << " *            OPERATTIONS ON ACCOUNT                  *" << endl;
	cout << " ******************************************************" << endl;
	SetConsoleTextAttribute(h, 11);
	cout << " [ENTER YOUR  ID] : "; cin >> account_ID;
	for (int i = 0; i < length; i++) {
		if (account_ID == arr[i].ID) {
			found = true;
			while (wrongPinCount < 6) {
				SetConsoleTextAttribute(h, 11);
				cout << " [ENTER YOUR PIN] : ";
				for (int i = 0; i < 4; i++) {
					_pin[i] = _getch();
					cout << '*';
					pin += _pin[i];
				}
				if (pin == Account[i].PIN) {
					system("cls");
					while (true) {
						SetConsoleCursorPosition(h, { 0, 1 });
						SetConsoleTextAttribute(h, 10);
						cout << " ******************************************************" << endl;
						cout << " *            OPERATTIONS ON ACCOUNT                  *" << endl;
						cout << " ******************************************************" << endl;
						SetConsoleCursorPosition(h, { 0,4 });
						for (int i = 0; i < 4; i++) {
							if (i == pointer) {
								SetConsoleTextAttribute(h, 139);
								cout << "---> " << menu[i] << endl;
							}
							else {
								SetConsoleTextAttribute(h, 11);
								cout << "     " << menu[i] << endl;
							}
						}
						key = _getch();
						if (key == 224) {
							key = _getch();
							if (key == 72) {
								pointer--;
								if (pointer == -1) pointer = 3;
							}
							else if (key == 80) {
								pointer++;
								if (pointer == 4) pointer = 0;
							}
						}
						else if (key == 13) {
							switch (pointer) {
							case 0: {
								Increase_Your_Balance(Account, i);
								Back_to_Menu();
								return *arr; }
							case 1: {
								Decrease_Your_Balance(Account, i);
								Back_to_Menu();
								return *arr; }
							case 2: {
								system("cls");
								return *arr;
							}
							case 3: {
								saveToFile();
								exit(0);
							}
							}
						}
					}
				}
				else {
					system("cls");
					SetConsoleCursorPosition(h, { 0, 1 });
					SetConsoleTextAttribute(h, 12);
					cout << "*************************************" << endl;
					cout << "*           WRONG PIN               *" << endl;
					cout << "*       PLEASE TRY AGAIN            *" << endl;
					cout << "*************************************" << endl;
					wrongPinCount++;
					pin.clear();
				}
				if (wrongPinCount == 3) { timeOut(); }
			}
			break;
		}
	}
	if (!found) {
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 12);
		cout << " *************************************" << endl;
		cout << " *       WRONG ACCOUNT ID            *" << endl;
		cout << " *       PLEASE TRY AGAIN            *" << endl;
		cout << " *************************************" << endl;
		Back_to_Menu();
	}
	return *arr;
}

/*
Bu funksiya USER-dən string tipli account_ID dəyişəninə dəyər alıb onu Account struct-da axtarıb tapdıqdan sonra
string tipli "pin" dəyişəninə char tipli "_pin[4]" massivinin simvollarını toplayıb uyğun Account-un elementinin
PİN-i ilə yoxlayır. _pin[4] massivinin hər simvolu getch() funksiyası ilə girildikdə ekrana "*" simvolu çap olunur.
Girilənlər doğru olduğu halda bool tipli found dəyişəni true olur, Hesabın məlumatları ekrana çap olunur və funksiya
işini başa vurur. Əks halda USER-ə "WRONG ID", "WRONG PIN" kimi mesajlar verirlir. USER İD-ni doğru girib PİN-i 3 dəfə
yanlış girdiyi halda timeOut() funksiyası işə düşür. Təkrar 3 dəfə yanlış girdiyi halda isə funksiya heç bir əməliyyat
yerinə yetirmədən sonlanır.
*/
void Check_Your_Balance() {
	system("cls");
	short wrongPinCount = 0;
	string account_ID, pin;
	char _pin[4];
	bool found = false;
	SetConsoleCursorPosition(h, { 0, 1 });
	SetConsoleTextAttribute(h, 10);
	cout << " ******************************************************" << endl;
	cout << " *               CHECK YOUR BALANCE                   *" << endl;
	cout << " ******************************************************";
	SetConsoleCursorPosition(h, { 0, 5 });
	SetConsoleTextAttribute(h, 11);
	cout << "\n [ENTER YOUR ACCOUNT ID] : "; cin >> account_ID;
	for (int i = 0; i < length; i++) {
		if (account_ID == Account[i].ID) {
			while (wrongPinCount < 6) {
				SetConsoleTextAttribute(h, 11);
				cout << "\n [ENTER YOUR PIN] : ";
				for (int j = 0; j < 4; j++) {
					_pin[j] = _getch();
					cout << '*';
					pin += _pin[j];
				}
				if (pin == Account[i].PIN) {
					system("cls");
					SetConsoleCursorPosition(h, { 0, 1 });
					SetConsoleTextAttribute(h, 10);
					cout << " ******************************************************" << endl;
					cout << " *               YOUR ACCOUNT DETAILS                 *" << endl;
					cout << " ******************************************************";
					SetConsoleCursorPosition(h, { 0, 5 });
					SetConsoleTextAttribute(h, 11);
					cout << " ACCOUNT ID : " << Account[i].ID << endl;
					cout << " BALANCE    : " << setprecision(2) << fixed << Account[i].Balance << " AZN";
					found = true;
					return;
				}
				else {
					system("cls");
					SetConsoleCursorPosition(h, { 0, 1 });
					SetConsoleTextAttribute(h, 12);
					cout << "*************************************" << endl;
					cout << "*           WRONG PIN               *" << endl;
					cout << "*       PLEASE TRY AGAIN            *" << endl;
					cout << "*************************************" << endl;
					wrongPinCount++;
					pin.clear();
				}
				if (wrongPinCount == 3) { timeOut(); }
			}
		}
	}

	if (!found) {
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 12);
		cout << " *************************************" << endl;
		cout << " *       WRONG ACCOUNT ID            *" << endl;
		cout << " *       PLEASE TRY AGAIN            *" << endl;
		cout << " *************************************" << endl;
		return;
	}
}

/*
Bu funksiya istifadəçidən payer_ID, receiver_ID və pin dəyişəni alıb onların Account struct-da mövcud olub-olmadığını
yoxladıqdan sonra USER-dən məbləğ girməsini gözləyir. Əgər girilən məbləğ 0-dan böyük və eyni zamanda müvafiq Account
elementinin (payer_İD-in) Balance-dakı dəyərindən kiçik-bərabər olduqda Balance-ı girilən amount qədər azaldıb,
receiver_ID-in Balance-ın dəyərinə eyni amount-u əlavə edir. Əks halda USER-ə müvafiq yanlış əməliyyat haqda - girilən
məbləğ 0-dan kiçik olması, payer_İD-in Balance-dakı dəyərinin girilən amount-dan az olması haqda məlumat verilir.
İstifadəçi payer_İD-ni, pin-i yalnış girdiyi təqdirdə istifadəçiyə uyğun məlumat verilib funksiya işini sonlandırır.
Əgər istifadəçi receiver_İD-ni yanlış girdikdə, girilən məbləğ 0-dan kiçik olduqda və ya müvafiq Account elementinin
Balance-dakı dəyərindən böyük olduqda History strukturunun növbəti elementinə uyğun enum tipi (status) olaraq yazılır.
İstifadəçi PİN-i 3 dəfə yalnış girərsə, timeOut() funksiyası işə düşür, 6-dəfə yalnış girərsə, funksiya heç bir iş görmədən sonlanır.
*/
void Money_Transfer() {
	now = time(0);
	currentime = *localtime(&now);
	string payer_ID, receiver_ID, pin;
	double amount = 0.0;
	short int payerIndex = 0, receiverIndex = 0, wrongPinCount = 0;
	bool payerFound = false, receiverFound = false;
	char _pin[4];
	system("cls");
	SetConsoleCursorPosition(h, { 0, 1 }); SetConsoleTextAttribute(h, 10);
	cout << " ******************************************************" << endl;
	cout << " *               MONEY TRANSFER                      *" << endl;
	cout << " ******************************************************" << endl;
	SetConsoleTextAttribute(h, 11); SetConsoleCursorPosition(h, { 0, 6 });
	cout << " [ENTER YOUR ACCOUNT ID] : "; cin >> payer_ID;
	for (int i = 0; i < length; i++) {
		if (payer_ID == Account[i].ID) {
			payerIndex = i;
			payerFound = true;
			while (wrongPinCount < 6) {
				SetConsoleTextAttribute(h, 11);
				cout << " [ENTER YOUR PIN] : ";
				for (int i = 0; i < 4; i++) {
					_pin[i] = _getch();
					cout << '*';
					pin += _pin[i];
				}
				if (pin == Account[i].PIN) {
					cout << "\n [ENTER RECEIVER ACCOUNT ID] : ";
					cin >> receiver_ID;
					for (int j = 0; j < length; j++) {
						if (receiver_ID == Account[j].ID) {
							receiverIndex = j;
							receiverFound = true;
							cout << " [ENTER AMOUNT] : ";
							cin >> amount;
							if (amount <= 0) {
								system("cls");
								SetConsoleCursorPosition(h, { 0, 1 });
								SetConsoleTextAttribute(h, 12);
								cout << " *************************************" << endl;
								cout << " *            ERROR!!!               *" << endl;
								cout << " *************************************" << endl;
								History[historyLength].payerId = Account[payerIndex].ID;
								History[historyLength].receiverId = Account[receiverIndex].ID;
								History[historyLength].amount = 0;
								History[historyLength].time.tm_mday = currentime.tm_mday;
								History[historyLength].time.tm_mon = currentime.tm_mon + 1;
								History[historyLength].time.tm_year = currentime.tm_year + 1900;
								History[historyLength].weekDay = time(0);
								History[historyLength].status = _ERROR;
								historyLength++;
								return;
							}
							else if (amount > 0 && amount > Account[payerIndex].Balance) {
								system("cls");
								SetConsoleCursorPosition(h, { 0, 1 });
								SetConsoleTextAttribute(h, 12);
								cout << " *************************************" << endl;
								cout << " *  NO ENOUGH FUNDS ON YOUR ACCOUNT  *" << endl;
								cout << " *   PLEASE INCREASE YOUR BALANCE    *" << endl;
								cout << " *************************************" << endl;
								SetConsoleTextAttribute(h, 11);
								cout << " \n YOUR BALANCE : " << setprecision(2) << fixed << Account[payerIndex].Balance << " AZN" << endl;
								History[historyLength].payerId = Account[payerIndex].ID;
								History[historyLength].receiverId = Account[receiverIndex].ID;
								History[historyLength].amount = amount;
								History[historyLength].time.tm_mday = currentime.tm_mday;
								History[historyLength].time.tm_mon = currentime.tm_mon + 1;
								History[historyLength].time.tm_year = currentime.tm_year + 1900;
								History[historyLength].weekDay = time(0);
								History[historyLength].status = NO_BALANCE;
								historyLength++;
								return;
							}
							else if (amount > 0 && amount < Account[payerIndex].Balance) {
								Account[payerIndex].Balance -= amount;
								Account[receiverIndex].Balance += amount;
								system("cls");
								SetConsoleCursorPosition(h, { 0, 1 });
								SetConsoleTextAttribute(h, 10);
								cout << " ****************************************" << endl;
								cout << " *  TRANSACTION COMPLETED SUCCESSFULLY  *" << endl;
								cout << " ****************************************" << endl;
								cout << "\n RECEIVER ACCOUNT ID : " << Account[receiverIndex].ID;
								cout << "\n TRANSFERED AMOUNT   : " << setprecision(2) << fixed << amount << " AZN";
								cout << "\n YOUR BALANCE        : " << setprecision(2) << fixed << Account[payerIndex].Balance << " AZN" << endl;
								History[historyLength].payerId = Account[payerIndex].ID;
								History[historyLength].receiverId = Account[receiverIndex].ID;
								History[historyLength].amount = amount;
								History[historyLength].time.tm_mday = currentime.tm_mday;
								History[historyLength].time.tm_mon = currentime.tm_mon + 1;
								History[historyLength].time.tm_year = currentime.tm_year + 1900;
								History[historyLength].weekDay = time(0);
								History[historyLength].status = SUCCESS;
								historyLength++;
								return;
							}
						}
					}
					if (!receiverFound) {
						system("cls");
						SetConsoleCursorPosition(h, { 0, 1 });
						SetConsoleTextAttribute(h, 12);
						cout << " *************************************" << endl;
						cout << " *      WRONG RECEIVER ID!           *" << endl;
						cout << " *       PLEASE TRY AGAIN            *" << endl;
						cout << " *************************************" << endl;
						History[historyLength].payerId = Account[payerIndex].ID;
						History[historyLength].receiverId = '\0';
						History[historyLength].amount = 0.0;
						History[historyLength].status = WRONG_RECEIVER_ID;
						History[historyLength].time.tm_mday = currentime.tm_mday;
						History[historyLength].time.tm_mon = currentime.tm_mon + 1;
						History[historyLength].time.tm_year = currentime.tm_year + 1900;
						History[historyLength].weekDay = time(0);
						historyLength++;
						return;
					}
				}
				else {
					system("cls");
					SetConsoleCursorPosition(h, { 0, 1 });
					SetConsoleTextAttribute(h, 12);
					cout << "*************************************" << endl;
					cout << "*           WRONG PIN               *" << endl;
					cout << "*       PLEASE TRY AGAIN            *" << endl;
					cout << "*************************************" << endl << endl;
					wrongPinCount++;
					pin.clear();
				}
				if (wrongPinCount == 3) { timeOut(); }
			}
			break;
		}
	}
	if (!payerFound) {
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 12);
		cout << " *************************************" << endl;
		cout << " *       WRONG ACCOUNT ID            *" << endl;
		cout << " *       PLEASE TRY AGAIN            *" << endl;
		cout << " *************************************" << endl;
		return;
	}
}

/*
Bu funksiya USER-dən string tipli account_ID və pin aldıqdan sonra onlari ardıcıl olaraq Account strukturunda
tapdiqdan sonra, istifadəçiyə "SUCCESSFULL TRANSACTIONS ONLY", "WRONG TRANSACTIONS ONLY", "ALL TRANSACTIONS"
kimi menyu seçimləri təklif edir. İstifadəçi _pin[4] massivinin hər simvolunu getch() funksiyası ilə girdikdə
ekrana "*" simvolu çap olunur. Menyu da istifadəçinin seçiminə uyğun olaraq "displaySuccessTransactions()" ,
"displayWrongTransactions()" funksiyaları icra olunur. Sonuncu seçimdə isə enum tipindən asılı olmayaraq girilən
Account İD-in Money_Transfer() funksiyasi ilə icra olunan bütün əməliyyatları (History struktrunun uygun elementləri)
ekrana çap olunur.
*/
void Display_History() {
	string menu[3] = { "SUCCESSFULL TRANSACTIONS ONLY", "WRONG TRANSACTIONS ONLY", "ALL TRANSACTIONS" };
	string account_ID, pin, STATUS;
	char _pin[4];
	bool found = false;
	short pointer = 0, key = 0, wrongPinCount = 0;
	SetConsoleCursorPosition(h, { 0, 1 });
	SetConsoleTextAttribute(h, 10);
	cout << " ***************************************************" << endl;
	cout << " *            TRANSACTIONS HISTORY                 *" << endl;
	cout << " ***************************************************" << endl;
	SetConsoleTextAttribute(h, 11);
	cout << " [ENTER YOUR ID] : "; cin >> account_ID;
	for (int i = 0; i < length; i++) {
		if (account_ID == Account[i].ID) {
			found = true;
			while (wrongPinCount < 6) {
				SetConsoleTextAttribute(h, 11);
				cout << " [ENTER PIN] : ";
				for (int i = 0; i < 4; i++) {
					_pin[i] = _getch();
					cout << '*';
					pin += _pin[i];
				}
				if (pin == Account[i].PIN) {
					while (true)
					{
						SetConsoleCursorPosition(h, { 0, 1 });
						SetConsoleTextAttribute(h, 10);
						cout << " ***************************************************" << endl;
						cout << " *            TRANSACTIONS HISTORY                 *" << endl;
						cout << " ***************************************************" << endl;
						for (int i = 0; i < 3; i++) {
							if (i == pointer) {
								SetConsoleTextAttribute(h, 139);
								cout << "---> " << menu[i] << endl;
							}
							else {
								SetConsoleTextAttribute(h, 11);
								cout << "     " << menu[i] << endl;
							}
						}
						Sleep(100);
						key = _getch();
						if (key == 224) {
							key = _getch();
							if (key == 72) {
								pointer--;
								if (pointer == -1) pointer = 2;
							}
							else if (key == 80) {
								pointer++;
								if (pointer == 3) pointer = 0;
							}
						}
						else if (key == 13) {
							switch (pointer) {
							case 0: {
								displaySuccessTransactions(account_ID);
								return;
							}
							case 1: {displayWrongTransactions(account_ID);
								return;
							}
							case 2: {
								system("cls");
								SetConsoleCursorPosition(h, { 0, 1 });
								SetConsoleTextAttribute(h, 10);
								cout << " ******************************************************************************" << endl;
								cout << " *                              ALL TRANSACTIONS                              *" << endl;
								cout << " ******************************************************************************" << endl << endl;
								printf("  |%11s |%12s |%13s |%11s |%19s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
								SetConsoleTextAttribute(h, 15);
								cout << "-----------------------------------------------------------------------------" << endl;
								for (int i = 0, j = 1; i < historyLength; i++) {
									if (History[i].payerId == account_ID) {
										cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
											<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
										printf("%02d.%02d.%d |", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year);
										if (History[i].status == 1) STATUS = "SUCCESS";
										else if (History[i].status == 2) STATUS = "NO BALANCE";
										else if (History[i].status == 3) STATUS = "WRONG RECEIVER ID";
										else STATUS = "ERROR";
										cout << setw(19) << STATUS << " |" << endl;
										j++;
									}
								}
								SetConsoleTextAttribute(h, 10);
								system("PAUSE");
								return;
							}
							}
						}
					}
				}
				else {
					system("cls");
					SetConsoleCursorPosition(h, { 0, 1 });
					SetConsoleTextAttribute(h, 12);
					cout << "*************************************" << endl;
					cout << "*           WRONG PIN               *" << endl;
					cout << "*       PLEASE TRY AGAIN            *" << endl;
					cout << "*************************************" << endl;
					wrongPinCount++;
					pin.clear();
				}
				if (wrongPinCount == 3) {
					timeOut();
					system("cls");
				}
			}
		}
	}
	if (!found) {
		system("cls");
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 12);
		cout << " *************************************" << endl;
		cout << " *       WRONG ACCOUNT ID            *" << endl;
		cout << " *       PLEASE TRY AGAIN            *" << endl;
		cout << " *************************************" << endl;
		system("PAUSE");
	}
}

/*
Bu funksiya string accountID parametrini qəbul edir. Money_Transfer() funksiyasında yerinə yetirilmiş və yalnız müvəffəqiyyətlə
tamamlanmış əməliyyatların "ONLY TODAY", "LAST SEVEN DAYS", "ONLY THIS MONTH", "ALL" menyu tiplərinə (switch case-lərinə) uyğun
olaraq ekrana çıxarılması işini görür. Funksiya icara olunan zaman "struct tm currentime" dəyişəninə cari zamani mənimsədib
case-lərdəki şərtlərə uyğun olaraq (günlük, 7 günlük, aylıq, hamısı) yalnız SUCCESS (1) enum tipinə mıxsus History struct-ın
elementləri (əməliyyatları) ekrana çap edir.
*/
void displaySuccessTransactions(string accountID) {
	system("cls");
	now = time(NULL);
	currentime = *localtime(&now);
	string menu[4] = { "ONLY TODAY", "LAST SEVEN DAYS", "ONLY THIS MONTH", "ALL" };
	int pointer = 0, key = 0;
	while (true) {
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 10);
		cout << " ***************************************************" << endl;
		cout << " *            SUCCESSFULL TRANSACTIONS             *" << endl;
		cout << " ***************************************************" << endl << endl;
		for (int i = 0; i < 4; i++) {
			if (i == pointer) {
				SetConsoleTextAttribute(h, 139);
				cout << "---> " << menu[i] << endl;
			}
			else {
				SetConsoleTextAttribute(h, 11);
				cout << "     " << menu[i] << endl;
			}
		}
		Sleep(100);
		key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) {
				pointer--;
				if (pointer == -1)
					pointer = 3;
			}
			else if (key == 80) {
				pointer++;
				if (pointer == 4)
					pointer = 0;
			}
		}
		else if (key == 13) {
			system("cls");
			switch (pointer) {
			case 0: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " **********************************************************************" << endl;
				cout << " *                         DAILY TRANSACTIONS                         *" << endl;
				cout << " **********************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%11s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-----------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if ((accountID == History[i].payerId) && (History[i].status == 1) && (History[i].time.tm_mday == currentime.tm_mday)
						&& (History[i].time.tm_mon == currentime.tm_mon + 1) && (History[i].time.tm_year == currentime.tm_year + 1900)) {
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |%11s |\n", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year, "SUCCESS");
						j++;
					}
				}
				cout << "-----------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				break;
			}
			case 1: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " **********************************************************************" << endl;
				cout << " *                          LAST SEVEN DAYS                           *" << endl;
				cout << " **********************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%11s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-----------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if (accountID == History[i].payerId && History[i].status == 1 && (now - History[i].weekDay <= (60 * 60 * 24 * 7)))
					{
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |%11s |\n", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year, "SUCCESS");
						j++;
					}
				}
				cout << "-----------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				break;
			}
			case 2: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " **********************************************************************" << endl;
				cout << " *                            THIS MONTH                              *" << endl;
				cout << " **********************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%11s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-----------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if (accountID == History[i].payerId && History[i].status == 1 && History[i].time.tm_mon == currentime.tm_mon + 1
						&& History[i].time.tm_year == currentime.tm_year + 1900)
					{
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |%11s |\n", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year, "SUCCESS");
						j++;
					}
				}
				cout << "-----------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				break;
			}
			case 3: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " **********************************************************************" << endl;
				cout << " *                          ALL  TRANSACTIONS                         *" << endl;
				cout << " **********************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%11s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-----------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if (accountID == History[i].payerId && History[i].status == 1)
					{
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |%11s |\n", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year, "SUCCESS");
						j++;
					}
				}
				cout << "-----------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				system("cls");
				break;
			}
			}
			return;
		}
	}
}

/*
Bu funksiya string accountID parametrini qəbul edir. Money_Transfer() funksiyasında yerinə yetirilmiş və müvəffəqiyyətlə
tamamlanmış əməliyyatlardan başqa bütün digər əməliyyatların "ONLY TODAY", "LAST SEVEN DAYS", "ONLY THIS MONTH", "ALL" menyu
tiplərinə (switch case-lərinə) uyğun olaraq ekrana çıxarılması işini görür. Funksiya icra olunan zaman "struct tm currentime"
dəyişəninə cari zamani mənimsədib case-lərdəki şərtlərə uyğun olaraq (günlük, 7 günlük, aylıq, hamısı), yalnız SUCCESS (1) enum-u
istisna olmaqla, bütün digər History struct-ın elementlərini (əməliyyatları) ekrana çap edir.
*/
void displayWrongTransactions(string accountID) {
	system("cls");
	string STATUS;
	now = time(NULL);
	currentime = *localtime(&now);
	string menu[4] = { "ONLY TODAY", "LAST SEVEN DAYS", "ONLY THIS MONTH", "ALL" };
	int pointer = 0, key = 0;
	while (true) {
		SetConsoleCursorPosition(h, { 0, 1 });
		SetConsoleTextAttribute(h, 10);
		cout << " ***************************************************" << endl;
		cout << " *               FAILED TRANSACTIONS               *" << endl;
		cout << " ***************************************************" << endl << endl;
		for (int i = 0; i < 4; i++) {
			if (i == pointer) {
				SetConsoleTextAttribute(h, 139);
				cout << "---> " << menu[i] << endl;
			}
			else {
				SetConsoleTextAttribute(h, 11);
				cout << "     " << menu[i] << endl;
			}
		}
		Sleep(100);
		key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) {
				pointer--;
				if (pointer == -1) pointer = 3;
			}
			else if (key == 80) {
				pointer++;
				if (pointer == 4) pointer = 0;
			}
		}
		else if (key == 13) {
			system("cls");
			switch (pointer) {
			case 0: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " ******************************************************************************" << endl;
				cout << " *                            DAILY TRANSACTIONS                              *" << endl;
				cout << " ******************************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%19s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-------------------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if ((accountID == History[i].payerId) && (History[i].status != 1) && (History[i].time.tm_mday == currentime.tm_mday)
						&& (History[i].time.tm_mon == currentime.tm_mon + 1) && (History[i].time.tm_year == currentime.tm_year + 1900)) {
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year);
						if (History[i].status == 2) STATUS = "NO BALANCE";
						else if (History[i].status == 3) STATUS = "WRONG RECEIVER ID";
						else STATUS = "ERROR";
						cout << setw(19) << STATUS << " |" << endl;
						j++;
					}
				}
				cout << "-------------------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				break;
			}
			case 1: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " ******************************************************************************" << endl;
				cout << " *                            LAST SEVEN DAYS                                 *" << endl;
				cout << " ******************************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%19s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-------------------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if (accountID == History[i].payerId && History[i].status != 1 && (now - History[i].weekDay <= (60 * 60 * 24 * 7))) {
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year);
						if (History[i].status == 2) STATUS = "NO BALANCE";
						else if (History[i].status == 3) STATUS = "WRONG RECEIVER ID";
						else STATUS = "ERROR";
						cout << setw(19) << STATUS << " |" << endl;
						j++;
					}
				}
				cout << "-------------------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				break;
			}
			case 2: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " ******************************************************************************" << endl;
				cout << " *                              THIS MONTH                                    *" << endl;
				cout << " ******************************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%19s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-------------------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if (accountID == History[i].payerId && History[i].status != 1 && History[i].time.tm_mon == currentime.tm_mon + 1
						&& History[i].time.tm_year == currentime.tm_year + 1900) {
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year);
						if (History[i].status == 2) STATUS = "NO BALANCE";
						else if (History[i].status == 3) STATUS = "WRONG RECEIVER ID";
						else STATUS = "ERROR";
						cout << setw(19) << STATUS << " |" << endl;
						j++;
					}
				}
				cout << "-------------------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				break;
			}
			case 3: {
				SetConsoleCursorPosition(h, { 0, 1 });
				SetConsoleTextAttribute(h, 10);
				cout << " ******************************************************************************" << endl;
				cout << " *                          ALL  TRANSACTIONS                                 *" << endl;
				cout << " ******************************************************************************" << endl << endl;
				printf("  |%11s |%12s |%13s |%11s |%19s |\n", "PAYER ID", "RECEIVER ID", "AMOUNT", "DATE", "STATUS");
				SetConsoleTextAttribute(h, 15);
				cout << "-------------------------------------------------------------------------------" << endl;
				for (int i = 0, j = 1; i < historyLength; i++) {
					if (accountID == History[i].payerId && History[i].status != 1) {
						cout << setw(2) << j << '|' << setw(11) << History[i].payerId << " |" << setw(12) << History[i].receiverId
							<< " |" << setprecision(2) << fixed << setw(9) << History[i].amount << " AZN | ";
						printf("%02d.%02d.%d |", History[i].time.tm_mday, History[i].time.tm_mon, History[i].time.tm_year);
						if (History[i].status == 2) STATUS = "NO BALANCE";
						else if (History[i].status == 3) STATUS = "WRONG RECEIVER ID";
						else STATUS = "ERROR";
						cout << setw(19) << STATUS << " |" << endl;
						j++;
					}
				}
				cout << "-------------------------------------------------------------------------------\n\n" << endl;
				system("PAUSE");
				break;
			}
			}
			return;
		}
	}
}


/*
Bu funksiya programın müxtəlif mərhələlərində istifadəçi tərəfindən PIN-in 3 dəfə  yanlış girildiyi
təqdirdə programın işini 2 dəqiqəlik müddətə dayandırır. _sleep(1000) funksiyası sayəsində seconds
dəyişəni 1 saniyədə 1 dəfə dikriment olunur, hər 60 dikrementden sonra minutes dəyişəni 1 dəfə dikriment olunur
və minutes dəyişəni 0-dan kiçik olana qədər dövr davam edir.
*/
void timeOut() {
	int minutes = 1, seconds = 60;
	while (minutes >= 0) {
		seconds--;
		SetConsoleCursorPosition(h, { 0,5 });
		SetConsoleTextAttribute(h, 14);
		cout << "*************************************" << endl;
		printf("*    [Timeout] : %02d:%02d              *\n", minutes, seconds);
		cout << "*************************************" << endl;
		Sleep(1000);
		if (seconds == 0) {
			minutes--;
			seconds = 60;
		}
	}
	return;
}
