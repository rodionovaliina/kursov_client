/**
* @file md5.h
* @author Rodionova
* @version 1.0
* @date 06.04.2023
* @copyright ИБСТ ПГУ
* @brief Заголовочный файл для модуля md5
*/

#include <cryptopp/hex.h> // HexEncoder
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h> // md5

using namespace std;

string MD5_hash(string hsh);
