//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>

// Funkcje konieczne do zamiany naglówków.
std::vector<char> headerChanger(std::vector<bool> tab);
std::vector<bool> headerBinChanger(std::vector<char> tab);
std::vector<bool> binary(std::vector<char> value);
std::vector<char> decimal(std::vector<bool> value1);

// Wypisywanie wektora.
template <typename T>
void wypisz(std::vector<T> tab);

// Funkcje wykorzystywane do zamiany liczb.
std::vector<bool> dec_to_bin(int liczba, int wielkosc);
int bin_to_dec(std::vector<bool> tab);


// Operacje wykonywane na argumentach, ktorych wyniki i ID przesylane sa w naglowku.
int add(int a, int b, int c);
int subtract(int a, int b, int c);
int divide(int a, int b, int c);
int multiply(int a, int b, int c);
int whichMax(int a, int b, int c);
int whichMin(int a, int b, int c);
int multiply(int a, int b, int c);
int silnia(int arg);
bool isEqual(int a, int b, int c);


// Zmienna odpowiedzialna za numerowanie klientow i nadawanie ID.
int id_int = 0;

int main() {

    int n = 0;
    std::cout << "Na ilu maksymalnych klientow chcesz pozwolic?" << std::endl << "> ";
    std::cin >> n;

    WSAData wsaData; // Zmienna potrzebna do otworzenia socketa.
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaData) != 0) {

        // Warunek pozwalający na podstawową obsługę błędów przy starcie winsocka.
        MessageBoxA(NULL, "Start Winsocka zostal przerwany.", "Blad", MB_OK | MB_ICONERROR);

        // Wyświetla okienko dialogowe.
        exit(1);
    }

    // Struktura odpowiedzialna za adresację IP.
    SOCKADDR_IN addr;
    int addrlen = sizeof(addr);
    // W przypadku łączenia dwóch komputerów, tutaj wpisujemy swój własny adres jako serwer.
    // Na komputerze klienckim wpisujemy adres serwera.
    // W przypadku korzystania z protokołu na komputerze lokalnym używamy adresu loopback: 127.0.0.1 na obu wersjach aplikacji.
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    // Socket odpowiedzialny za nasłuchiwanie.
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);

    // Przypisujemy adres do socketa odpowiedzialnego za nasłuchiwanie.
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));

    // Funkcja odpowiedzialna za nasłuchiwanie.
    listen(sListen, SOMAXCONN);

    std::cout << "Serwer wlaczony. Oczekuje na polaczenia.  \nAby zamknac, wyslij kombinacje klawiszy CTRL+C." << std::endl << std::endl;

    // Socket odpowiedzialny za połączenie z klientem.
    SOCKET newConnection;


    for (int i = 0; i < n; i++) {

        newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);

        // Zmienna odpowiedzialna za numerowanie klientów.
        int clientNum;


        if (newConnection == 0) {
            std::cout << "Wystapil blad zwiazany z brakiem polaczenia pomiedzy serwerem a klientem.";
        }
        else {

            // Numerowanie klienta.
            clientNum = id_int;
            std::cout << std::endl << "Klient " << clientNum << " podlaczony." << std::endl;

            std::vector<bool> header;

            // Pole operacji 
            std::vector<bool> operacje = { 0,0,0,0 };

            std::vector<bool> status = { 0,0,0 };

            // Liczba odpowiadająca AKTUALNEJ długości danych, którą umieszczany w nagłówku. Długość ta może ulec zmianie.
            int dataLength = 6 + 64 + 3;

            // Zamiana długości pola danych z liczby dziesiętnej na binarną z pomocą tablicy.
            std::vector<bool> vecDataLength = dec_to_bin(dataLength, 64);
            // Pole ID.
            // Zamiana ID zapisanego w postaci dziesiętnej na binarną z pomocą tablicy.
            std::vector<bool> ID = dec_to_bin(id_int, 6);

            // Pole długość nagłówka.
            // Całkowita długość nagłówka. Początkowy nagłówek, będzie miał długość 112 bitów, czyli 14 bajtów.
            int headerLength = 148; //112
            std::vector<bool> headerLengthVec = dec_to_bin(headerLength, 64);

            // Pole wielkość dopełnienia.
            // Określa wielkość dopełnienia do wielokrotności liczby 8. Zapisywane na trzech bitach.
            int fill = 0;
            std::vector<bool> fillVec = dec_to_bin(fill, 3);

            // Wypełnienie pola operacji nagłówka zerami - klient jeszcze nie podjął decyzji
            // którą z 8 operacji będzie chciał wykonać.
            for (int i = 0; i < 4; i++) {
                header.push_back(0);
            }

            // Wypełnienie pola statusu nagłówka.
            for (int i = 0; i < 3; i++) {
                header.push_back(status[i]);
            }

            // Wypełnienie pola długość danych nagłówka.
            for (int i = 0; i < 64; i++) { //32
                header.push_back(vecDataLength[i]);
            }

            // Wypełnienie pola ID nagłówka.
            for (int i = 0; i < 6; i++) {
                header.push_back(ID[i]);
            }

            // Wypełnienie pola długość nagłówka.
            for (int i = 0; i < 64; i++) {
                header.push_back(headerLengthVec[i]);
            }

            // Wypełnienie pola długość dopełnienia.
            for (int i = 0; i < 3; i++) {
                header.push_back(fillVec[i]);
            }

            // Ustawienie statusu na 1 - Wysyłany zostaje header z wypełnionymi polami ID oraz status.
            status[0] = 0;
            status[1] = 0;
            status[2] = 1;

            // Wypełnienie pola status nagłówka.
            for (int i = 4; i < 7; i++) {
                header[i] = status[i - 4];
            }
            // Konwersja vector<bool> -> vector<char> z pomocą funkcji.
            std::vector<char> headerCharToSend = decimal(header);

            // Przesłanie klientowi faktycznego nagłówka.
            send(newConnection, &headerCharToSend[0], headerCharToSend.size(), NULL);

            /*

            Operacje wykonywane po stronie klienta...

            */
            std::cout << "Odbieram dane..." << std::endl;

            // Odbieranie nagłówka z wpisaną nową długością nagłówka.
            std::vector<char> headerCharReceived(18);
            recv(newConnection, &headerCharReceived[0], headerCharReceived.size(), NULL);
            std::vector<bool> headerBinRecv = binary(headerCharReceived);

            // Wyłuskanie wartości długości pola danych.
            for (int i = 0; i < 64; i++) {
                vecDataLength[i] = headerBinRecv[i + 7];
            }
            dataLength = bin_to_dec(vecDataLength);
            for (int i = 0; i < 64; i++) {
                headerLengthVec[i] = headerBinRecv[i + 77];
            }
            headerLength = bin_to_dec(headerLengthVec);
            for (int i = 0; i < 3; i++) {
                fillVec[i] = headerBinRecv[i + 141];
            }
            fill = bin_to_dec(fillVec);
            // Modyfikacja długości nagłówka na rzecz odebrania pełnych danych od klienta.
            std::vector<char> headerArgRcv(headerLength / 8);
            recv(newConnection, &headerArgRcv[0], headerArgRcv.size(), NULL);
            headerBinRecv = binary(headerArgRcv);
            headerBinRecv.resize(headerLength - fill);
            std::cout << std::endl;
            headerLength -= fill;
            dataLength -= fill;
            // Modyfikacja pola status. Status 4 - odebrano dane od klienta.
            status[0] = 1;
            status[1] = 0;
            status[2] = 0;

            // Uzupełnienie pól nagłówka: status oraz długość.
            for (int i = 4; i < 7; i++) {
                headerBinRecv[i] = status[i - 4];
            }
            // Modyfikacja lokalnej wartości zmiennej długość pola danych. Zmiana o sumę rozmiarów trzech argumentów.
            int wielkoscArgumentu = (dataLength - 81) / 3;

            // Modyfikacja lokalnej tablicy odpowiedzialnej za reprezentację pola operacji.
            operacje[0] = headerBinRecv[0];
            operacje[1] = headerBinRecv[1];
            operacje[2] = headerBinRecv[2];
            operacje[3] = headerBinRecv[3];

            // Zamiana argumentów z nagłówka, na liczby dziesiętne.
            std::vector<bool> argX;
            std::vector<bool> argY;
            std::vector<bool> argZ;
            std::vector<bool> argument;

       
            for (int i = 0; i < wielkoscArgumentu; i++) {
                argX.push_back(headerBinRecv[i + 144]);
            }
            for (int i = 0; i < wielkoscArgumentu; i++) {
                argY.push_back(headerBinRecv[i + 144 + wielkoscArgumentu]);
            }
            for (int i = 0; i < wielkoscArgumentu; i++) {
                argZ.push_back(headerBinRecv[i + 144 + wielkoscArgumentu + wielkoscArgumentu]);
            }
            for (int i = 0; i < 8; i++) {
                argument.push_back(headerBinRecv[i + 144 + wielkoscArgumentu + wielkoscArgumentu + wielkoscArgumentu]); /// tu pamietac zmienic nastepnym razem
            }
  
            int argX_int = bin_to_dec(argX);
            int argY_int = bin_to_dec(argY);
            int argZ_int = bin_to_dec(argZ);
            int argument_int = bin_to_dec(argument);
            std::cout << "Pierwsza liczba: " << argX_int << std::endl
                << "Druga liczba: " << argY_int << std::endl
                << "Trzecia liczba: " << argZ_int << std::endl;
            // Tablica w której przechowywany jest wynik. Jej wielkość ustawiany jest na potrojoną wartość rozmiaru argumentów.
            std::vector<bool> vecResult(wielkoscArgumentu * 3);

            // W zależności od podanej operacji wykonujemy zestaw określonych funkcji.
            // Jedyną różnicą jest operacja odejmowania, w której ostatni bit liczby ustalamy jako kod znaku.
            if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 0 && operacje[3] == 0) {
                std::cout << "Wynik potegowania: " << add(argX_int, argY_int, argZ_int) << std::endl;
                vecResult = dec_to_bin(add(argX_int, argY_int, argZ_int), vecResult.size());
            }
            else if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 0 && operacje[3] == 1) {
                std::cout << "Wynik dzielenia: " << divide(argX_int, argY_int, argZ_int) << std::endl;
                vecResult = dec_to_bin(divide(argX_int, argY_int, argZ_int), vecResult.size());
                if (divide(argX_int, argY_int, argZ_int) < 0) vecResult[0] = 1;
            }
            else if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 1 && operacje[3] == 0) {
                std::cout << "Wynik mnozenie: " << multiply(argX_int, argY_int, argZ_int) << std::endl;
                vecResult = dec_to_bin(multiply(argX_int, argY_int, argZ_int), vecResult.size());
            }
            else if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 1 && operacje[3] == 1) {
                std::cout << "Wynik odejmowania: " << subtract(argX_int, argY_int, argZ_int) << std::endl;
                vecResult = dec_to_bin(subtract(argX_int, argY_int, argZ_int), vecResult.size());
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 0 && operacje[2] == 0) {
                std::cout << "Ktora wieksza: " << whichMax(argX_int, argY_int, argZ_int) << std::endl;
                vecResult = dec_to_bin(whichMax(argX_int, argY_int, argZ_int), vecResult.size());
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 0 && operacje[2] == 1) {
                std::cout << "Ktora mniejsza: " << whichMin(argX_int, argY_int, argZ_int) << std::endl;
                vecResult = dec_to_bin(whichMin(argX_int, argY_int, argZ_int), vecResult.size());
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 1 && operacje[3] == 0) {
                std::cout << "Czy liczby sa rowne? " << isEqual(argX_int, argY_int, argZ_int) << std::endl;
                vecResult[0] = isEqual(argX_int, argY_int, argZ_int);
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 1 && operacje[2] == 1) {
                std::cout << "Wynik dodawania: " << add(argX_int, argY_int, argZ_int) << std::endl;
                vecResult = dec_to_bin(add(argX_int, argY_int, argZ_int), vecResult.size());
            }




            // Modyfikacja lokalnego pola długość pola danych.
            dataLength += wielkoscArgumentu * 3;
            headerLength += wielkoscArgumentu * 3;
            // Maksymalna  wielkość tablicy odpowiadającej za przechowywanie wyniku silnii
            // to 10 ze względu na szybko rosnacą złożoność obliczeniową.
            std::vector<bool> resultSilnia(10);

            for (int i = 0; i < 10; i++) {
                resultSilnia[i] = 0;
            }
            // Obliczanie silnii.
            if (argument_int == 1) {
                if (argX_int > 6) {
                    std::cout << std::endl << "Wartosc obliczonej silnii jest zbyt duza, zeby przeslac ja do klienta." << std::endl;
                }
                else {
                    resultSilnia = dec_to_bin(silnia(argX_int), 10);
                    std::cout << "Silnia z " << argX_int << " = " << silnia(argX_int);
                }
            }
            else if (argument_int == 2) {
                if (argY_int > 6) {
                    std::cout << std::endl << "Wartosc obliczonej silnii jest zbyt duza, zeby przeslac ja do klienta." << std::endl;
                }
                else {
                    resultSilnia = dec_to_bin(silnia(argY_int), 10);
                    std::cout << "Silnia z " << argY_int << " = " << silnia(argY_int) << std::endl;
                }
            }
            else if (argument_int == 3) {
                if (argZ_int > 6) {
                    std::cout << std::endl << "Wartosc obliczonej silnii jest zbyt duza, zeby przeslac ja do klienta." << std::endl;
                }
                else {
                    resultSilnia = dec_to_bin(silnia(argZ_int), 10);
                    std::cout << "Silnia z " << argZ_int << " = " << silnia(argZ_int) << std::endl;
                }
            }
            std::vector<char> headerCharToSendResult = decimal(headerBinRecv);

            std::cout << std::endl;
            // Zwiekszenie liczby odpowiadającej długości pola danych.
            dataLength += 10;
            headerLength += 10;
            fill = 8 - (headerLength % 8);
            headerLength += fill;
            dataLength += fill;
            vecDataLength = dec_to_bin(dataLength, 64); //32
            fillVec = dec_to_bin(fill, 3);
            for (int i = 7; i < 71; i++) {
                headerBinRecv[i] = vecDataLength[i - 7];
            }
            for (int i = 77; i < 141; i++) {
                headerBinRecv[i] = headerLengthVec[i - 77];
            }
            for (int i = 141; i < 144; i++) {
                headerBinRecv[i] = fillVec[i - 141];
            }
            // Uzupełnienie nagłówka odpowiednimi danymi.
            for (int i = 7; i < 71; i++) {
                headerBinRecv[i] = vecDataLength[i - 7];
            }
            for (int i = 0; i < vecResult.size(); i++) {
                headerBinRecv.push_back(vecResult[i]);
            }
            for (int i = 0; i < resultSilnia.size(); i++) {
                headerBinRecv.push_back(resultSilnia[i]);
            }

            // Ustawienie statusu 5 - wysłanie ostatecznych danych.
            status[0] = 1;
            status[1] = 0;
            status[2] = 1;

            for (int i = 4; i < 7; i++) {
                headerBinRecv[i] = status[i - 4];
            }

            // Przesłanie ostatecznych danych wynikowych w postaci nagłówka (w postaci tablicy char utworzonej z tablicy bool).
            headerCharToSendResult = decimal(headerBinRecv);
            send(newConnection, &headerCharToSendResult[0], headerCharToSendResult.size(), NULL);
            std::cout << std::endl;
            // Zwiększenie liczby ID klienta dla potencjalnego następnego klienta.
            id_int++;

            std::cout << "Zamykanie polaczenia z klientem: " << clientNum << std::endl << std::endl;
            closesocket(newConnection);
        }
    }
    return 0;
}

std::vector<char> headerChanger(std::vector<bool> tab) {
    std::vector<char> header(tab.size());
    for (int i = 0; i < tab.size(); i++) {
        if (tab[i] == 0) {
            header[i] = '0';
        }
        else if (tab[i] == 1) {
            header[i] = '1';
        }
    }
    return header;
}

std::vector<bool> headerBinChanger(std::vector<char> tab) {
    std::vector<bool> header(tab.size());
    for (int i = 0; i < tab.size(); i++) {
        if (tab[i] == '0') {
            header[i] = 0;
        }
        else if (tab[i] == '1') {
            header[i] = 1;
        }
    }
    return header;
}

std::vector<bool> dec_to_bin(int liczba, int wielkosc)
{
    std::vector<bool> tab(wielkosc);


    int i = wielkosc - 1;
    while (liczba) //dopóki liczba będzie różna od zera
    {
        tab[i--] = liczba % 2;
        liczba /= 2;
    }
    return tab;
}


int bin_to_dec(std::vector<bool> tab) {
    int wielkosc = tab.size();
    int dec = 0;
    for (int i = 0; i < wielkosc; i++) {
        int pom = tab[i] * pow(2, wielkosc - i - 1);
        dec += pom;
    }
    return dec;
}
// funkcje matematyczne
int add(int a, int b, int c) {
    return a + b + c;
}
int subtract(int a, int b, int c) {
    return a - b - c;
}
int divide(int a, int b, int c) {
    return a / (b * c);
}
int multiply(int a, int b, int c) {
    return a * b * c;
}
int whichMax(int a, int b, int c) {
    if (a > b > c) return a;
    if (a > c > b) return a;
    if (b > a > c) return b;
    if (b > c > a) return b;
    if (c > a > b) return c;
    if (c > b > a) return c;
}
int whichMin(int a, int b, int c) {
    if (a > b > c) return c;
    if (a > c > b) return b;
    if (b > a > c) return c;
    if (b > c > a) return a;
    if (c > a > b) return b;
    if (c > b > a) return a;
}
bool isEqual(int a, int b, int c) {
    if (a == b == c) return false; else return true;
}
int multiplying(int a, int b, int c) {
    return a ^ b;
}
int silnia(int arg) {
    if (arg < 2) return arg;
    return arg * silnia(arg - 1);
}

std::vector<char> decimal(std::vector<bool>  value1) {
    std::vector<char> value;
    if (value1.size() % 8 == 0) {
        value.resize(value1.size() / 8);
        for (int i = 0; i < value.size(); i++) {
            int pom2 = 8 * i;
            std::vector<bool> pom(8);
            for (int j = 0; j < 8; j++) {
                pom[j] = value1[pom2 + j];
            }
            value[i] = bin_to_dec(pom);
        }
    }
    else {
        int pom = value1.size() % 8;
        int dopelnienie = 8 - pom;
        value.resize((value1.size() + dopelnienie) / 8);
        for (int i = 0; i < value.size(); i++) {
            int pom2 = 8 * i;
            std::vector<bool> vecpom(8);
            for (int j = 0; j < 8; j++) {
                if (pom2 + j >= value1.size()) {
                    vecpom[j] = 0;
                }
                else vecpom[j] = value1[pom2 + j];
            }
            value[i] = bin_to_dec(vecpom);
        }
    }
    return value;
}

std::vector<bool> binary(std::vector<char> value) {
    std::vector<bool> header;
    std::vector<bool> pom(8);
    for (int i = 0; i < value.size(); i++) {
        unsigned char k = value[i];
        int test = k;
        pom = dec_to_bin(test, 8);
        for (int j = 0; j < pom.size(); j++) {
            header.push_back(pom[j]);
        }
    }
    return header;
}

template<typename T>
void wypisz(std::vector<T> tab) {
    for (int i = 0; i < tab.size(); i++) {
        std::cout << tab[i];
    }
}
