#include "Crypto.h"

void Crypto::init()
{
    if (sodium_init() < 0) 
    {
        throw std::exception("Can`t initialize sodium");
    }
}
void Crypto::generateKeyPair(const std::vector<std::string>& macs, std::vector<unsigned char>& private_key, std::vector<unsigned char>& public_key)
{
    std::string seed_str;
    for (const auto& mac : macs) seed_str += mac;

    std::vector<unsigned char> seed(crypto_generichash_BYTES);
    crypto_generichash(seed.data(), seed.size(),
        reinterpret_cast<const unsigned char*>(seed_str.c_str()),
        seed_str.size(), nullptr, 0);

    public_key.resize(crypto_sign_PUBLICKEYBYTES);
    private_key.resize(crypto_sign_SECRETKEYBYTES);
    crypto_sign_seed_keypair(public_key.data(), private_key.data(), seed.data());
}

std::string Crypto::hashPassword(const std::string& password)
{
    std::vector<unsigned char> hash(crypto_pwhash_STRBYTES);
    crypto_pwhash_str(reinterpret_cast<char*>(hash.data()),
        password.c_str(), password.size(),
        crypto_pwhash_OPSLIMIT_SENSITIVE,
        crypto_pwhash_MEMLIMIT_SENSITIVE);
    return std::string(reinterpret_cast<char*>(hash.data()));
}
bool Crypto::verifyPassword(const std::string& password, const std::string& hash)
{
    return crypto_pwhash_str_verify(hash.c_str(),
        password.c_str(),
        password.size()) == 0;
}

std::string Crypto::base64Encode(const std::vector<unsigned char>& data) 
{
    std::string result;
    size_t encoded_len = sodium_base64_encoded_len(data.size(), sodium_base64_VARIANT_URLSAFE_NO_PADDING);
    result.resize(encoded_len);
    sodium_bin2base64(
        const_cast<char*>(result.data()), encoded_len,
        data.data(), data.size(),
        sodium_base64_VARIANT_URLSAFE_NO_PADDING
    );
    return result;
}