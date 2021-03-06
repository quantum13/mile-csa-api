//
// Created by lotus mile on 03/10/2018.
//

#ifndef MILECSA_MILECSA_TRANSACTION_HPP
#define MILECSA_MILECSA_TRANSACTION_HPP

#include <string>
#include <optional>
#include <functional>

#include "mile_crypto.h"
#include "json.hpp"

#include "milecsa_assets.hpp"
#include "milecsa_result.hpp"
#include "milecsa_error.hpp"
#include "milecsa_pair.hpp"


namespace milecsa{
    namespace transaction {

        constexpr const size_t description_max_length = 62;

        /**
         * Digest calculator closure
         */
        typedef std::function<void(milecsa::keys::DigestCalculator &calculator)> CalculatorClosure;

        /**
         * Request builder defines preperaing transactions body protocol.
         * Creates a request body, computes transaction signadure and digest.
         * @tparam T basic serializer class, by default is nlohmann::json
         */
        template<typename T>
        class Request {
        public:

            typedef Request<T> RequestBuilder;

            typedef std::function<void(
                    T &parameters,
                    const std::string &publicKey)> ParamsClosure;

            /**
             * Request builder constructor.
             *
             * @param keyPair - wallet keys pair
             * @param blockId - current block id
             * @param transactionId - user defined transaction id
             * @param assetCode - asset code
             */
            Request(const milecsa::keys::Pair &keyPair,
                    const uint256_t blockId,
                    const uint64_t  transactionId,
                    const milecsa::token &asset = milecsa::assets::NILL) :
                    keyPair_(keyPair),
                    signer_(keyPair_),
                    blockId_(blockId),
                    transactionId_(transactionId),
                    asset_(asset){}

            /**
             *
             * Prepared request's body getter.
             *
             * @return optional body serializer. Class T must have subscript operator.
             */
            const std::optional<T> get_body() const {
                if (parameters_.empty()) return std::nullopt;
                return std::make_optional(parameters_);
            };

            /**
             * Get prepared and signed transaction digest.
             *
             * @return optional digest string
             */
            const std::optional<std::string> get_digest() const {
                if (digest_.IsZero()) return std::nullopt;
                return std::make_optional(digest_.ToBase58CheckString());
            }

            /**
             * Get requested transaction body signature.
             *
             * @return optional signature string
             */
            const std::optional<std::string> get_signature() const {
                if (signature_.IsZero()) return std::nullopt;
                return std::make_optional(signature_.ToBase58CheckString());
            }

        public:

            /**
             *
             * Setup request body in inheritance classes.
             *
             * @param calculatorClosure - ...
             * @param paramsClosure - ...
             */
            void setup(const std::string &transactionName, const CalculatorClosure &calculatorClosure, const ParamsClosure &paramsClosure) {

                digestCalculator_.Initialize();

                digestCalculator_.Update(transactionId_);
                digestCalculator_.Update(blockId_);

                if (asset_.code != milecsa::assets::NILL.code)
                    digestCalculator_.Update(asset_.code);

                digestCalculator_.Update(keyPair_.get_public_key());

                calculatorClosure(digestCalculator_);

                digestCalculator_.Finalize(digest_);
                signer_.SignDigest(digest_, signature_)  ;

                parameters_ =
                        {
                                {"transaction-type", transactionName},
                                {"block-id",       UInt256ToDecString(blockId_)},
                                {"transaction-id", transactionId_},
                                {"digest",         digest_.ToBase58CheckString()},
                                {"signature",      signature_.ToBase58CheckString()}
                        };

                paramsClosure(parameters_, keyPair_.get_public_key().encode());
            }

            
            /**
             * Copy constructor
             *
             * @param request - copied request
             */
            Request(const Request& request):
            parameters_(request.parameters_),keyPair_(request.keyPair_),
            signer_(request.signer_),blockId_(request.blockId_),transactionId_(request.transactionId_),
            asset_(request.asset_),digest_(request.digest_),signature_(request.signature_),
            digestCalculator_(request.digestCalculator_){}

        private:
            Request(){};

            T parameters_;

            keys::Pair keyPair_;
            keys::Signer signer_;

            uint256_t blockId_;
            uint64_t  transactionId_;
            milecsa::token  asset_;

            ::Digest digest_;
            ::Signature signature_;

            milecsa::keys::DigestCalculator digestCalculator_;
        };

        /**
         * Json request type
         */
        typedef std::optional<Request<nlohmann::json>> JsonRequest;
    }
}


#endif //MILECSA_MILECSA_TRANSACTION_HPP
