#!/bin/sh

#  genkey.sh
# create private and public key
#
#

[ -d keydir ] || mkdir keydir

cd keydir

openssl genrsa -out sheldon_privatekey.pem 1024
openssl req -new -key sheldon_privatekey.pem -out sheldon_certrequest.csr
openssl x509 -req -in sheldon_certrequest.csr -signkey sheldon_privatekey.pem -out sheldon_certificate.pem

cd ..