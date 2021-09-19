//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>

int dopelnienie = 0;
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


int main() {

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
    // Adres IP komputera
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    // Wybrany dowolny port, który nie jest zajęty.
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    // Socket odpowiedzialny za połączenie z serwerem.
    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);

    // Podstawowa obsługa błędów w przypadku braku połączenia pomiędzy serwerem, a klientem.
    if (connect(Connection, (SOCKADDR*)&addr, addrlen) != 0) {
        MessageBoxA(NULL, "Blad polaczenia!", "Blad", MB_OK | MB_ICONERROR);
        return 0;
    }
    else {
        std::cout << "Polaczony!" << std::endl;
        std::cout << "Wybierz działanie z dostepnych: " << std::endl <<
            "0 0 0 0 - potegowanie," << std::endl <<
            "0 0 0 1 - dzielenie" << std::endl <<
            "0 0 1 0 - mnozenie" << std::endl <<
            "0 0 1 1 - odejmowanie" << std::endl <<
            "0 1 0 0 - ktora wieksza" << std::endl <<
            "0 1 0 1 - ktora mniejsza" << std::endl <<
            "0 1 1 0 - czy rowne" << std::endl <<
            "0 1 1 1 - dodawanie" << std::endl;



        // Deklaracja nagłówka typu vector<char> i odebranie go od serwera.
        std::vector<char> headerCharReceived(18);
        recv(Connection, &headerCharReceived[0], headerCharReceived.size(), NULL);

        // Zamiana odebranej tablicy charów z wypełnionym nagłówkiem, na tablicę bool.
        std::vector<bool> headerBinRecv = binary(headerCharReceived);

        // Pole statusu.
        std::vector<bool> status(3);

        // Ustawienie statusu na 2 - odebrano nagłówek z przypisanym klientowi ID, statusem oraz ówczesnym rozmiarem pola danych.
        status[0] = 0;
        status[1] = 1;
        status[2] = 0;

        // Pole operacji. Dostępne operacje wyświetlane są przy uruchomieniu aplikacji klienckiej.
        std::vector<char> operacjeOdUzytkownika(4);
        if (status[1] == 1) {
            std::cout << "Podaj identyfikator operacji." << std::endl;
            std::cout << "> ";
            for (int i = 0; i < 4; i++) {
                std::cin >> operacjeOdUzytkownika[i];
            }
        }


        // Zamiana pobranego identyfikatora operacji z typu vector<char> na typ vector<bool>.
        std::vector<bool> operacje = headerBinChanger(operacjeOdUzytkownika);
        for (int i = 0; i < 4; i++) {
            headerBinRecv[i] = operacje[i];
        }
        // Aktualizacja pola status nagłówka.
        for (int i = 4; i < 7; i++) {
            headerBinRecv[i] = status[i - 4];
        }

        // Podanie liczby bitów, na których ma być zapisana każda z liczb.
        int wielkosc;
        std::cout << "Na ilu bitach zapisana ma byc kazda z liczb?" << std::endl << "> ";
        std::cin >> wielkosc;

        int flag;
        int argX_int;
        do {
            std::cout << "Podaj pierwsza liczbe calkowita, dodatnia: " << std::endl << "> ";
            std::cin >> argX_int;
            if (argX_int > pow(2, wielkosc) - 1) {
                std::cout << "Podaj liczbe, ktora mozna zapisac na " << wielkosc << " bitach. Maksymalna mozliwa liczba w Twoim przypadku: "
                    << pow(2, wielkosc) - 1 << std::endl;
                flag = 0;
            }
            else flag = 1;
        } while (flag == 0);

        int flagY;
        int argY_int;
        do {
            std::cout << "Podaj druga liczbe calkowita, dodatnia: " << std::endl << "> ";
            std::cin >> argY_int;
            if (argY_int > pow(2, wielkosc) - 1) {
                std::cout << "Podaj liczbe, ktora mozna zapisac na " << wielkosc << " bitach. Maksymalna mozliwa liczba w Twoim przypadku: "
                    << pow(2, wielkosc) - 1 << std::endl;
                flagY = 0;
            }
            else flagY = 1;
        } while (flagY == 0);

        int flagZ;
        int argZ_int;
        do {
            std::cout << "Podaj trzecia liczbe calkowita, dodatnia: " << std::endl << "> ";
            std::cin >> argZ_int;
            if (argZ_int > pow(2, wielkosc) - 1) {
                std::cout << "Podaj liczbe, ktora mozna zapisac na " << wielkosc << " bitach. Maksymalna mozliwa liczba w Twoim przypadku: "
                    << pow(2, wielkosc) - 1 << std::endl;
                flagZ = 0;
            }
            else flagZ = 1;
        } while (flagZ == 0);


        // Konwersja argumentów z postaci dziesiętnej na binarną, konieczną do przesłania serwerowi.
        std::vector<bool> argX = dec_to_bin(argX_int, wielkosc);
        std::vector<bool> argY = dec_to_bin(argY_int, wielkosc);
        std::vector<bool> argZ = dec_to_bin(argZ_int, wielkosc);

        // Aktualizacja pola rozmiar pola danych nagłówka.
        // Ówczesne pole długości pola danych. Do tej pory uzupełnione zostało tylko o ID (nastąpił tylko jeden przekaz nagłówka).
        int dataLength = 73 + (3 * wielkosc) + 8;
        std::vector<bool> vecDataLength = dec_to_bin(dataLength, 64);



        // Pole długość nagłówka.
        // Całkowita długość nagłówka. Początkowy nagłówek, będzie miał długość 144 bitów, czyli 18 bajtów. Następny powiększony będzie o potrojoną wielkość bitówą argumentów liczbowych oraz 8 bitowe pole argumentu silnii.
        int headerLength = 144 + (3 * wielkosc) + 8;
        std::vector<bool> headerLengthVec = dec_to_bin(headerLength, 64);

        // Pole wielkość dopełnienia.
        // Określa wielkość dopełnienia do wielokrotności liczby 8. Zapisywane na trzech bitach.
        int fill = 0;
        std::vector<bool> fillVec = dec_to_bin(fill, 3);


        // Ustawienie statusu na 3 - Wysłanie nagłówka z przypisanym klientowi ID, statusem aktualnym rozmiarem pola danych, operacjami oraz argumentami
        status[0] = 0;
        status[1] = 1;
        status[2] = 1;

        // Wypełnienie nagłówka odpowiednimi danymi.
        for (int i = 4; i < 7; i++) {
            headerBinRecv[i] = status[i - 4];
        }

        // Wysłanie nagłówka ze zaktualizowanym polem długości danych i długości nagłówka.
        std::vector<char> headerCharToSend = decimal(headerBinRecv);
        fill = 8 - (headerLength % 8);
        if (fill == 8) { fill = 0; }
        headerLength += fill;
        dataLength += fill;
        std::cout << std::endl;
        fillVec = dec_to_bin(fill, 3); //0
        vecDataLength = dec_to_bin(dataLength, 64);
        headerLengthVec = dec_to_bin(headerLength, 64);

        for (int i = 7; i < 71; i++) {
            headerBinRecv[i] = vecDataLength[i - 7];
        }
        for (int i = 77; i < 141; i++) {
            headerBinRecv[i] = headerLengthVec[i - 77];
        }
        for (int i = 141; i < 144; i++) {
            headerBinRecv[i] = fillVec[i - 141];
        }
        headerCharToSend = decimal(headerBinRecv);

        std::cout << std::endl;
        send(Connection, &headerCharToSend[0], headerCharToSend.size(), NULL);
        // Wypełnienie pola argumentów.
        for (int i = 0; i < wielkosc; i++) {
            headerBinRecv.push_back(argX[i]);
        }
        for (int i = wielkosc; i < wielkosc + wielkosc; i++) {
            headerBinRecv.push_back(argY[i - wielkosc]);
        }
        for (int i = wielkosc + wielkosc; i < wielkosc + wielkosc + wielkosc; i++) {
            headerBinRecv.push_back(argZ[i - wielkosc - wielkosc]);
        }

        int argument;
        if (status[1] == 1) {
            std::cout << "Podaj argument na ktorym ma byc wykonana operacja silni." << std::endl << "> ";
            std::cin >> argument;
        }
        std::vector<bool> vecArg = dec_to_bin(argument, 8);
        for (int i = 0; i < 8; i++) {
            headerBinRecv.push_back(vecArg[i]);
        }

        // Zamiana tablicy booli na tablicę charów, która jest potrzebna do wysłania.

        headerCharToSend = decimal(headerBinRecv);
        // Przesłanie serwerowi wielkości nagłówka, oraz samego nagłówka.
        send(Connection, &headerCharToSend[0], headerCharToSend.size(), NULL);
        /*

        Serwer wykonuje operacje na danych...

        */
        //Zwiększenie wielkości pola danych i wielkości nagłówka o pole wyniku operacji (2*wielkość bitowa argumentów) oraz o 10 bitowy wynik operacji silni.       
        dataLength += (3 * wielkosc) - fill + 10;
        headerLength += (3 * wielkosc) - fill + 10;
        // Ostateczna konwersja nagłówka z postaci char na bool wraz z przypisaniem odpowiedniej długości.
        std::vector<char> headerCharReceivedFinal((headerLength + (headerLength % 8)) / 8);
        recv(Connection, &headerCharReceivedFinal[0], (headerLength + (headerLength % 8)) / 8, NULL);
        std::cout << std::endl;
        std::vector<bool> headerBinRecvFinal = binary(headerCharReceivedFinal);
        // Ustawienie pola status na wartość 6 - odebrano dane finalne i zakończono komunikację z serwerem.
        status[0] = 1;
        status[1] = 1;
        status[2] = 0;
        headerLength = (headerLength + (headerLength % 8));
        // Modyfikacja długości nagłówka na rzecz odebrania pełnych danych od klienta.
        headerBinRecvFinal.resize(headerLength - fill);
        headerLength -= fill;
        dataLength -= fill;
        // Wypisanie na konsole odpowiednich danych wynikowych.
        std::vector<bool> result(3 * wielkosc);
        for (int i = 0; i < 3 * wielkosc; i++) {
            result[i] = headerBinRecvFinal[i + 152 + (wielkosc * 3)]; //tu 120
        }
        if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 1) {
            int zmiana = -1;
            if (result[0] == 1) {
                result[0] = 0;
                std::cout << "Wynik dzialania: " << zmiana * bin_to_dec(result) << std::endl;
            }
            else {
                std::cout << "Wynik dzialania: " << bin_to_dec(result);
            }
        }
        else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 1 && operacje[3] == 0) {
            if (result[0] == 1) std::cout << "Liczby sa sobie rowne."; else std::cout << "Liczby sie od siebie roznia.";
        }
        else {
            std::cout << "Wynik dzialania: " << bin_to_dec(result);
        }
        std::vector<bool> silniaResult(10);
        for (int i = 0; i < 10; i++) {
            silniaResult[i] = headerBinRecvFinal[i + 152 + wielkosc * 6];
        }
        if (bin_to_dec(silniaResult) == 0) std::cout << std::endl << "Silnia nie mogla zostac obliczona."; else
            std::cout << std::endl << "Wynik silnii: " << bin_to_dec(silniaResult);

    }
    std::cout << std::endl;
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


int bin_to_dec(std::vector<bool> tab) {
    int wielkosc = tab.size();
    int dec = 0;
    for (int i = 0; i < wielkosc; i++) {
        int pom = tab[i] * pow(2, wielkosc - i - 1);
        dec += pom;
    }
    return dec;
}

template<typename T>
void wypisz(std::vector<T> tab) {
    for (int i = 0; i < tab.size(); i++) {
        std::cout << tab[i];
    }
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
        dopelnienie = 8 - pom;
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
