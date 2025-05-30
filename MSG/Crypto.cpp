#include "Crypto.h"

void Crypto::init()
{
    if (sodium_init() < 0) 
    {
        throw std::exception("Can`t initialize sodium");
    }
}

std::string Crypto::encryptMAC(const std::string& mac, const std::vector<unsigned char>& key) 
{
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    std::vector<unsigned char> ciphertext(mac.size() + crypto_aead_aes256gcm_ABYTES);
    unsigned long long ciphertext_len;
    crypto_aead_aes256gcm_encrypt(
        ciphertext.data(), &ciphertext_len,
        reinterpret_cast<const unsigned char*>(mac.data()), mac.size(),
        nullptr, 0,
        nullptr, nonce, key.data()
    );

    std::vector<unsigned char> combined(nonce, nonce + sizeof(nonce));
    combined.insert(combined.end(), ciphertext.begin(), ciphertext.end());
    return base64Encode(combined);
}

std::vector<std::string> Crypto::encryptAllMACs(const std::vector<std::string>& macs,const std::vector<unsigned char>& key)
{
    std::vector<std::string> encrypted_macs;
    for (const auto& mac : macs) {
        encrypted_macs.push_back(encryptMAC(mac, key));
    }       
    return encrypted_macs;
}

std::string Crypto::decryptMAC(const std::string& base64_data, const std::vector<unsigned char>& key) 
{
    std::vector<unsigned char> combined = base64Decode(base64_data);

    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    std::copy(combined.begin(), combined.begin() + sizeof(nonce), nonce);
    std::vector<unsigned char> plaintext(combined.size() - sizeof(nonce) - crypto_aead_aes256gcm_ABYTES);
    unsigned long long plaintext_len;
    if (crypto_aead_aes256gcm_decrypt(
        plaintext.data(), &plaintext_len,
        nullptr,
        combined.data() + sizeof(nonce), combined.size() - sizeof(nonce),
        nullptr, 0,
        nonce, key.data()
    ) != 0) {
        throw std::runtime_error("Decryption failed\n");
    }

    return std::string(plaintext.begin(), plaintext.end());
}

std::string Crypto::hashPassword(const std::string& password)
{
    std::string hash;
    hash.resize(crypto_pwhash_STRBYTES);
    const int result = crypto_pwhash_str(
        &hash[0],  // C++17: hash.data()
        password.data(),
        password.size(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
    );
    hash.resize(std::strlen(hash.c_str()));
    return std::string(reinterpret_cast<char*>(hash.data()));
}

bool Crypto::verifyPassword(const std::string& password, const std::string& hash)
{
    return crypto_pwhash_str_verify(hash.c_str(),
        password.c_str(),
        password.size()) == 0;
}

std::vector<unsigned char> Crypto::generateKeySeed(
    const std::vector<std::string>& encrypted_macs_base64,
    const std::vector<unsigned char>& key
) {
    std::vector<std::string> macs;
    for (const auto& enc_mac : encrypted_macs_base64) {
        macs.push_back(decryptMAC(enc_mac, key));
    }
    std::sort(macs.begin(), macs.end());

    std::string combined;
    for (const auto& mac : macs) {
        combined += mac;
    }
    std::vector<unsigned char> seed(crypto_hash_sha256_BYTES);
    crypto_hash_sha256(seed.data(),
        reinterpret_cast<const unsigned char*>(combined.data()),
        combined.size());

    return seed;
}

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> Crypto::generateKeyPair(const std::vector<unsigned char>& seed) 
{
    if (seed.size() != crypto_sign_SEEDBYTES) {
        throw std::runtime_error("Invalid seed size");
    }

    std::vector<unsigned char> public_key(crypto_sign_PUBLICKEYBYTES);
    std::vector<unsigned char> private_key(crypto_sign_SECRETKEYBYTES);
    crypto_sign_seed_keypair(public_key.data(), private_key.data(), seed.data());

    return { public_key, private_key };
}

std::vector<unsigned char> Crypto::deriveKeyFromPassword(
    const std::string& password,
    const std::vector<unsigned char>& salt
) {
    std::vector<unsigned char> key(crypto_aead_aes256gcm_KEYBYTES);

    if (crypto_pwhash(
        key.data(), key.size(),
        password.c_str(), password.size(),
        salt.data(), 
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE,
        crypto_pwhash_ALG_DEFAULT
    ) != 0) {
        throw std::runtime_error("Key derivation failed\n");
    }

    return key;
}

std::string Crypto::base64Encode(const std::vector<unsigned char>& data) {
    std::string encoded(data.size() * 2, '\0');
    size_t encoded_len;
    sodium_bin2base64(
        encoded.data(), encoded.size(),
        data.data(), data.size(),
        sodium_base64_VARIANT_ORIGINAL
    );
    encoded.resize(strlen(encoded.c_str())); 
    return encoded;
}

std::vector<unsigned char> Crypto::base64Decode(const std::string& encoded) {
    std::vector<unsigned char> decoded(encoded.size());
    size_t decoded_len;
    if (sodium_base642bin(
        decoded.data(), decoded.size(),
        encoded.c_str(), encoded.size(),
        nullptr, &decoded_len, nullptr,
        sodium_base64_VARIANT_ORIGINAL
    ) != 0) {
        throw std::runtime_error("Base64 decoding failed");
    }
    decoded.resize(decoded_len);
    return decoded;
}

std::vector<unsigned char> Crypto::generateSalt()
{
    std::vector<unsigned char> salt(crypto_pwhash_SALTBYTES); 
    randombytes_buf(salt.data(), salt.size());
    return salt;
}

std::vector<unsigned char> Crypto::generateChatKey() 
{
    std::vector<unsigned char> key(crypto_aead_aes256gcm_KEYBYTES);
    randombytes_buf(key.data(), key.size());
    return key;
}

std::vector<unsigned char> Crypto::generateKeySeed(const std::vector<std::string>& macs) {
    std::vector<std::string> sortedMacs = macs;
    std::sort(sortedMacs.begin(), sortedMacs.end());

    std::string combined;
    for (const auto& mac : sortedMacs) {
        combined += mac;
    }

    std::vector<unsigned char> seed(crypto_hash_sha256_BYTES);
    crypto_hash_sha256(seed.data(),
        reinterpret_cast<const unsigned char*>(combined.data()),
        combined.size());

    return seed;
}

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> Crypto::generateEncryptionKeyPair(const std::vector<std::string>& macs)
{
    auto seed = generateKeySeed(macs);
    if (seed.size() != crypto_box_SEEDBYTES)
    {
        throw std::runtime_error("Invalid seed size for encryption keys");
    }

    std::vector<unsigned char> publicKey(crypto_box_PUBLICKEYBYTES);
    std::vector<unsigned char> privateKey(crypto_box_SECRETKEYBYTES);
    crypto_box_seed_keypair(publicKey.data(), privateKey.data(), seed.data());

    return { publicKey, privateKey };
}

std::vector<unsigned char> Crypto::encryptAsymmetric(const std::vector<unsigned char>& publicKey, const std::vector<unsigned char>& message) 
{
    if (publicKey.size() != crypto_box_PUBLICKEYBYTES) {
        throw std::runtime_error("Invalid public key size");
    }

    std::vector<unsigned char> ciphertext(message.size() + crypto_box_SEALBYTES);
    crypto_box_seal(ciphertext.data(), message.data(), message.size(), publicKey.data());

    return ciphertext;
}

std::vector<unsigned char> Crypto::decryptAsymmetric(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& publicKey,
const std::vector<unsigned char>& privateKey)
{
    if (publicKey.size() != crypto_box_PUBLICKEYBYTES ||
        privateKey.size() != crypto_box_SECRETKEYBYTES) {
        throw std::runtime_error("Invalid key sizes");
    }

    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_box_SEALBYTES);
    if (crypto_box_seal_open(
        decrypted.data(),
        ciphertext.data(),
        ciphertext.size(),
        publicKey.data(),
        privateKey.data()
    ) != 0) {
        throw std::runtime_error("Decryption failed");
    }

    return decrypted;
}

std::string Crypto::encryptSymmetric(
    const std::string& message,
    const std::vector<unsigned char>& key)
{
    if (key.size() != crypto_aead_aes256gcm_KEYBYTES) {
        throw std::runtime_error("Invalid key size for symmetric encryption");
    }

    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    std::vector<unsigned char> ciphertext(
        message.size() + crypto_aead_aes256gcm_ABYTES
    );
    unsigned long long ciphertext_len;

    crypto_aead_aes256gcm_encrypt(
        ciphertext.data(), &ciphertext_len,
        reinterpret_cast<const unsigned char*>(message.data()), message.size(),
        nullptr, 0,  
        nullptr,      
        nonce, key.data()
    );

 
    std::vector<unsigned char> combined;
    combined.reserve(sizeof(nonce) + ciphertext_len);
    combined.insert(combined.end(), nonce, nonce + sizeof(nonce));
    combined.insert(combined.end(), ciphertext.data(), ciphertext.data() + ciphertext_len);

    return base64Encode(combined);
}

std::string Crypto::decryptSymmetric(
    const std::string& base64_ciphertext,
    const std::vector<unsigned char>& key)
{
    if (key.size() != crypto_aead_aes256gcm_KEYBYTES) {
        throw std::runtime_error("Invalid key size for symmetric decryption");
    }

  
    std::vector<unsigned char> combined = base64Decode(base64_ciphertext);


    const size_t min_size = crypto_aead_aes256gcm_NPUBBYTES + crypto_aead_aes256gcm_ABYTES;
    if (combined.size() < min_size) {
        throw std::runtime_error("Ciphertext too short after base64 decoding");
    }


    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    std::copy_n(combined.data(), sizeof(nonce), nonce);

   
    const unsigned char* encrypted_data = combined.data() + sizeof(nonce);
    const size_t encrypted_data_len = combined.size() - sizeof(nonce);

 
    std::vector<unsigned char> plaintext(encrypted_data_len);
    unsigned long long plaintext_len;

    if (crypto_aead_aes256gcm_decrypt(
        plaintext.data(), &plaintext_len,
        nullptr, 
        encrypted_data, encrypted_data_len,
        nullptr, 0, 
        nonce, key.data()
    ) != 0) {
        throw std::runtime_error("Symmetric decryption failed: authentication error");
    }

    
    return std::string(plaintext.begin(), plaintext.begin() + plaintext_len);
}
