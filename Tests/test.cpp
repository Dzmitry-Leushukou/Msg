#include "pch.h"
#include "../MSG/Client.h"
#include "../MSG/Image.h"

Image *image;
TEST(Client, InitGoodFile)
{
	EXPECT_THROW({ Client::init("D:\\Programming\\Projects\\MSG\\Tests\\Cases\\config1.txt"); }, std::runtime_error);
}
TEST(Client, InitNoFile)
{
	EXPECT_THROW({ Client::init("D:\\Programming\\Projects\\MSG\\Tests\\Cases\\"); }, std::invalid_argument);
}
TEST(Client, InitFileWtihoutIP)
{
	EXPECT_THROW({ Client::init("D:\\Programming\\Projects\\MSG\\Tests\\Cases\\config2"); }, std::invalid_argument);
}
TEST(Client, InitFileWtihBadPort)
{
	EXPECT_THROW({ Client::init("D:\\Programming\\Projects\\MSG\\Tests\\Cases\\config3"); }, std::invalid_argument);
}
TEST(Client, SendMessageWithoutConnection)
{
	EXPECT_THROW({ Client::userExist("aboba","aboba"); }, std::runtime_error);
}
TEST(Client, GetMac)
{
	ASSERT_NO_THROW({ Client::getMAC(); });
}
TEST(Client, ReceiveMessageWithoutConnection)
{
	EXPECT_THROW({ Client::getChatHeader("aboba");}, std::runtime_error);
}
TEST(Image, OpenNotExistImage)
{
	EXPECT_THROW({ image = new Image("bla","bla","bla","bla"); image->open(); }, std::invalid_argument);
}
TEST(Image, OpenExistImage)
{
	ASSERT_NO_THROW({ image = new Image("bla","bla","bla","bla"); image->save("aboba"); image->open(); });
}