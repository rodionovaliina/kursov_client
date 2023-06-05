#include <UnitTest++/UnitTest++.h>
#include "client.h"
using namespace std;

/*
 Для макроса TEST_FIXTURE можно объявить специальный класс, в конструкторе которого
будут выполняться действия, предваряющие тест, а в деструкторе — завершающие.
*/

struct Key_fixture {
    Client * p;
    Key_fixture()
    {
        p = new Client();
    }
    ~Key_fixture()
    {
        delete p;
    }
};

SUITE(Server)//Макрос. FIXTURE при одинаковых аргумиентах
{
    TEST_FIXTURE(Key_fixture, NormalTest) {
        //1 Удачный сценарий
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/vector.txt";
        p->resultat_file = "/home/stud/C++Projects/123144/build-Debug/bin/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/build-Debug/bin/config/vclient.conf";
        p->Server("127.0.0.1", "33333");
        CHECK(true);
    }

    TEST_FIXTURE(Key_fixture, ErrIp) {
        //2 Подключение к серверу. Введен не верный адрес
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/vector.txt";
        p->resultat_file = "/home/stud/C++Projects/123144/build-Debug/bin/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/build-Debug/bin/config/vclient.conf";
        CHECK_THROW(p->Server("2215024", "33333"), client_error);
    }

    TEST_FIXTURE(Key_fixture, ErrPort) {
        //3 Подключение к серверу. Введен не верный порт
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/vector.txt";
        p->resultat_file = "/home/stud/C++Projects/123144/build-Debug/bin/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/build-Debug/bin/config/vclient.conf";
        CHECK_THROW(p->Server("127.0.0.1", "3445"), client_error);
    }

    TEST_FIXTURE(Key_fixture, ErrPutyFileLogParol) {
        //4 Ошибка открытия файла с логинами и паролями
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/vector.txt";
        p->resultat_file = "//home/stud/C++Projects/123144/build-Debug/bin/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/3/12235/config/vclient.conf";;
        CHECK_THROW(p->Server("127.0.0.1", "33333"), client_error);
    }


    TEST_FIXTURE(Key_fixture, ErrLogin) {
        //5 Ошибка идентификации. Введен не правильный логин
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/my.txt";
        p->resultat_file = "/home/stud/C++Projects/123144/build-Debug/bin/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/build-Debug/bin/Nouser.conf";
        CHECK_THROW(p->Server("127.0.0.1", "33333"), client_error);
    }

    TEST_FIXTURE(Key_fixture, ErrParol) {
        //6 Ошибка аутентификации. Введен не правильный пароль
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/vector.txt";
        p->resultat_file = "/home/stud/C++Projects/123144/build-Debug/bin/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/build-Debug/bin/NoPswd.conf";
        CHECK_THROW(p->Server("127.0.0.1", "33333"), client_error);
    }

    TEST_FIXTURE(Key_fixture, ErrPutyFileVectors) {
        //7 Ошибка открытия файла с векторами
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/12432.txt";
        p->resultat_file = "/home/stud/C++Projects/123144/build-Debug/bin/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/build-Debug/bin/config/vclient.conf";
        CHECK_THROW(p->Server("127.0.0.1", "33333"), client_error);
    }

   
        TEST_FIXTURE(Key_fixture, ErrPutyFileResultat) {
        //8 Ошибка открытия файла для записи суммы
        p->vector_file = "/home/stud/C++Projects/123144/build-Debug/bin/vector.txt";
        p->resultat_file = "/home/stud/C++Projects/123144/build-Debug/bin/142235/resultat.txt";
        p->autf_file = "/home/stud/C++Projects/123144/build-Debug/bin/config/vclient.conf";
        CHECK_THROW(p->Server("127.0.0.1", "33333"), client_error);
    }
}

int main(int argc, char **argv)
{
    return UnitTest::RunAllTests();
}
