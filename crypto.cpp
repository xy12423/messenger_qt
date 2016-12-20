#include "stdafx.h"
#include "crypto.h"

#ifndef _NO_CRYPTO

using namespace CryptoPP;

void ECC_crypto_helper::genKey(const char* privatekeyFile)
{
	ECIES<ECP>::PrivateKey &privateKey = d0.AccessKey();
	privateKey.GenerateRandom(prng, MakeParameters(Name::GroupOID(), CURVE));
	FileSink fs(privatekeyFile, true);
	privateKey.Save(fs);
}

void ECC_crypto_helper::initKey(const char* privatekeyFile)
{
	ECIES<ECP>::PrivateKey &privateKey = d0.AccessKey();
	try
	{
		FileSource fs(privatekeyFile, true);
		privateKey.Load(fs);
		if (!privateKey.Validate(prng, 3))
            genKey(privatekeyFile);
	}
	catch (CryptoPP::FileStore::OpenErr &)
	{
        genKey(privatekeyFile);
	}
	dh_priv_block_size = dh.PrivateKeyLength();
	dh_pub_block_size = dh.PublicKeyLength();
	dh_agree_block_size = dh.AgreedValueLength();
}

const CryptoPP::ECIES<CryptoPP::ECP>::Decryptor& ECC_crypto_helper::GetPublicKey()
{
	return d0;
}

std::string ECC_crypto_helper::GetPublicKeyString()
{
	std::string ret;
	StringSink buf(ret);
	ECIES<ECP>::Encryptor e0(d0);
	e0.GetPublicKey().Save(buf);

	return ret;
}

std::string ECC_crypto_helper::GetUserIDGlobal()
{
	std::string ret;
	StringSink buf(ret);
	ECIES<ECP>::Encryptor e0(d0);

	DL_PublicKey_EC<ECP>& key = dynamic_cast<DL_PublicKey_EC<ECP>&>(e0.AccessPublicKey());
	assert(&key != nullptr);

	key.DEREncodePublicKey(buf);
	assert(ret.front() == 4);
	ret.erase(0, 1);

	return ret;
}

void ECC_crypto_helper::encrypt(const std::string& src, std::string& dst, const ECIES<ECP>::Encryptor& e1)
{
	dst.clear();
	StringSource ss1(src, true, new PK_EncryptorFilter(prng, e1, new StringSink(dst)));
}

void ECC_crypto_helper::encrypt(const byte* src, size_t src_size, std::string& dst, const CryptoPP::ECIES<CryptoPP::ECP>::Encryptor& e1)
{
	dst.clear();
	StringSource ss1(src, src_size, true, new PK_EncryptorFilter(prng, e1, new StringSink(dst)));
}

void ECC_crypto_helper::decrypt(const std::string& src, std::string& dst, const CryptoPP::ECIES<CryptoPP::ECP>::Decryptor& _d0)
{
	dst.clear();
	StringSource ss1(src, true, new PK_DecryptorFilter(prng, _d0, new StringSink(dst)));
}

void ECC_crypto_helper::decrypt(const byte* src, size_t src_size, CryptoPP::SecByteBlock& dst, const CryptoPP::ECIES<CryptoPP::ECP>::Decryptor& _d0)
{
	_d0.Decrypt(prng, src, src_size, dst);
}

void ECC_crypto_helper::init_sym_encryption(CBC_Mode<AES>::Encryption& e, const SecByteBlock& key, SecByteBlock& iv)
{
	assert(key.SizeInBytes() == sym_key_size);
	prng.GenerateBlock(iv, sym_key_size);
	e.SetKeyWithIV(key, sym_key_size, iv);
}

void ECC_crypto_helper::init_sym_decryption(CBC_Mode<AES>::Decryption& d, const SecByteBlock& key, const SecByteBlock& iv)
{
	assert(key.SizeInBytes() == sym_key_size);
	assert(iv.SizeInBytes() == sym_key_size);
	d.SetKeyWithIV(key, sym_key_size, iv);
}

void ECC_crypto_helper::sym_encrypt(const std::string& src, std::string& dst, CBC_Mode<AES>::Encryption& e)
{
	dst.clear();
	StringSource ss(src, true, new StreamTransformationFilter(e, new StringSink(dst)));
}

void ECC_crypto_helper::sym_decrypt(const std::string& src, std::string& dst, CBC_Mode<AES>::Decryption& d)
{
	dst.clear();
	StringSource ss(src, true, new StreamTransformationFilter(d, new StringSink(dst)));
}

void ECC_crypto_helper::hash(const std::string& src, std::string& dst, size_t input_shift)
{
	CryptoPP::SHA512 hasher;
	char result[hash_size];
	memset(result, 0, sizeof(result));
	hasher.CalculateDigest(reinterpret_cast<byte*>(result), reinterpret_cast<const byte*>(src.data()), src.size() - input_shift);
	dst.append(result, hash_size);
}

void ECC_crypto_helper::dhGen(SecByteBlock& priv, SecByteBlock& pub)
{
	dh.GenerateKeyPair(prng, priv, pub);
}

bool ECC_crypto_helper::dhAgree(SecByteBlock& agree, const SecByteBlock& priv, const SecByteBlock& pub)
{
	CryptoPP::SHA256 hasher;
	SecByteBlock _agree(dh_agree_block_size);
	if (!dh.Agree(_agree, priv, pub))
		return false;
	assert(_agree.SizeInBytes() == dh_agree_block_size);
	hasher.CalculateDigest(agree, _agree, dh_agree_block_size);
	return true;
}

rand_num_type ECC_crypto_helper::genRandomNumber()
{
	byte t[sizeof(rand_num_type)];
	prng.GenerateBlock(t, sizeof(rand_num_type));
	return *reinterpret_cast<rand_num_type*>(t);
}

#endif
