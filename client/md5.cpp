//Crypto++
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>
/**
 * @file md5.cpp
 * @brief Файл для хэширования md5
 */
 
#include "md5.h"

using namespace CryptoPP;

/**
* @brief Получение хэша по алгоритму md5
* @param hsh сообщения получаемое для хэширования
* @return Полученный хэш
*/

string MD5_hash(string hsh)
{

    
    string massange; // оперделяем пременну для хранения хэша
    
    Weak1::MD5 hashmd4;  // Определяем переменную для типа хэша из библиотеки CryptoPP
    
    StringSource(hsh, true, // источник-стока
                    new HashFilter(hashmd4, // фильтр-"хеширователь"
                                    new HexEncoder( // кодировщик в строку цифр
                                        new StringSink(massange)))); // строка-приемник
                                        
    // выводим сообщение и его хэш
    return massange;
}

