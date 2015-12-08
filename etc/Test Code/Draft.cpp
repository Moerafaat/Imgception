int encryption_length;
qDebug() << RSA_size(key_pair);
char* encryption = new char(RSA_size(key_pair));
if((encryption_length = RSA_public_encrypt(strlen("Test Message"), (unsigned char*)"Test Message", (unsigned char*)encryption, key_pair, RSA_PKCS1_OAEP_PADDING)) == -1)
logMessage("Encryption Failed.");
qDebug() << encryption;
char* decryption = new char(encryption_length);
if(RSA_private_decrypt(encryption_length, (unsigned char*)encryption, (unsigned char*)decryption, key_pair, RSA_PKCS1_OAEP_PADDING) == -1)
logMessage("Decryption Failed.");
qDebug() << decryption;
