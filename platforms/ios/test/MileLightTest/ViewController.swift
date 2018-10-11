//
//  ViewController.swift
//  MileLightTest
//
//  Created by lotus mile on 22.08.2018.
//  Copyright © 2018 MILE. All rights reserved.
//

import UIKit
import MileCsaLight

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        do {

            let pks = try MileCsaKeys.random();

            Swift.print("PKS:  \(pks)")
            
            Swift.print("Validate public: \(MileCsaKeys.validatePublic("2qPBE5ZVqxFSKZyLmuaMdi1dSvbT2HffQP7XrTB6akXoeJz216"))")
            Swift.print("Validate private: \(MileCsaKeys.validatePrivate("9RX5j9APJtu5gSbD2VTrdhMSctcF9rwVFsoiWYawyPsB1HRJ4Eq1KC7f6ourpWaYSzLAk3qRrBiU7tjKAS7M4usFF6eRz"))")

//            Swift.print("Register   node:",  try MileCsa.registerNode(pks, address: "hello", transactionId: 0))
//            Swift.print("Unregister node: ", try MileCsa.unregisterNode(pks,transactionId: 0))
//
//            let destPks = try MileCsa.generateKeys()
//
//            Swift.print("Wallet       : ", try MileCsa.createTransfer(pks,
//                                                                        destPublicKey: destPks.publicKey,
//                                                                        transactionId: 0,
//                                                                        assets: 0,
//                                                                        amount: "0"))


            let pks_sph1 = try MileCsaKeys.withSecretPhrase("фраза какая-то")
            let pks_sph2 = try MileCsaKeys.withSecretPhrase("фраза какая-то")
            let pks_sph3 = try MileCsaKeys.withSecretPhrase("фраза какая-то другая")

            let pks_sph4 = try MileCsaKeys.fromPrivateKey(pks_sph3.privateKey)


            Swift.print(" pks with secret phrase1 = \(pks_sph1, pks_sph1==pks_sph2) ")
            Swift.print(" pks with secret phrase2 = \(pks_sph2, pks_sph1.privateKey==pks_sph2.privateKey) ")
            Swift.print(" pks with secret phrase3 = \(pks_sph3, pks_sph1==pks_sph3) ")

            Swift.print(" pks with private key phrase4 = \(pks_sph4, pks_sph3==pks_sph4) ")

//            Swift.print("Wallet       : ", try MileCsa.createTransfer(pks,
//                                                                        destPublicKey: "WTF!KEY",
//                                                                        transactionId: 0,
//                                                                        assets: 0,
//                                                                        amount: "0"))
        }
        catch let error {
            Swift.print("Error: \(error)")
        }
        
    }

}

