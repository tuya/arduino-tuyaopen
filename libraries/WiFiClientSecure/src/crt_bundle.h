#ifndef _CRT_BUNDLE_H_
#define _CRT_BUNDLE_H_

#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "tal_log.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Attach and enable use of a bundle for certificate verification
 *
 *
 * @param[in]  conf      The config struct for the SSL connection.
 *
 * @return
 *             - BK_OK  if adding certificates was successful.
 *             - Other   if an error occured or an action must be taken by the calling process.
 */
OPERATE_RET arduino_crt_bundle_attach(void *conf);


/**
 * @brief      Disable and dealloc the certification bundle
 *
 * Removes the certificate verification callback and deallocates used resources
 *
 * @param[in]  conf      The config struct for the SSL connection.
 */
void arduino_crt_bundle_detach(mbedtls_ssl_config *conf);


/**
 * @brief      Set the default certificate bundle used for verification
 *
 * Overrides the default certificate bundle. In most use cases the bundle should be
 * set through menuconfig. The bundle needs to be sorted by subject name since binary search is
 * used to find certificates.
 *
 * @param[in]  x509_bundle     A pointer to the certificate bundle.
 */
void arduino_crt_bundle_set(const uint8_t *x509_bundle);


#ifdef __cplusplus
}
#endif

#endif //_CRT_BUNDLE_H_
