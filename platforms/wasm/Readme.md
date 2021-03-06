# MILE CSA JavaScript API

## Wallet pair constructor

### Error handling

```javascript

    /**
    * Create an error container 
    */
    var error = new Module.Error()
    
    if( error.hasError ) {
        //
        // Handle error
        //
        // console.log( "Error: " + error.what )
    }

```

### Create wallet pair with random keys

```javascript

    /**
    * Create a new wallet pair with random keys  
    */
    var pair = new Module.Pair.Random( error )
    
    if( error.hasError ) {
        //
        // Handle Wallet pair generator error        
        //
        // console.log( "Error: " + error.what )
    }
    else {
        // access to public key:
        var public_key = pair.public_key
        var private_key = pair.private_key  
 
        //
        // Process pair
        //
        ...                
    }

```

### Create wallet pair from private key

```javascript
    var private_key = "....."
    var pair = new Module.Pair.FromPrivateKey( private_key, error )
    
    if( error.hasError ) {
        //
        // Handle Wallet pair generator error        
        //
        // console.log( "Error: " + error.what )
    }
    else {
        // access to public key:
        var public_key = pair.public_key
        var private_key = pair.private_key  

        //
        // Process pair
        //
        ...                
    }

```

### Keys validation

**Validate public key**
```javascript

    //
    // public key from INPUT FORM 
    //
    if( !Module.Pair.ValidatePublicKey( field.public_key, error ) ) {
        //
        // Handle error
        //
        // console.log( "Error: " + error.what )
    }
````

**Validate private key**
```javascript

    //
    // private key from INPUT FORM 
    //

    if( !Module.Pair.ValidatePrivateKey( field.private_key, error ) ) {
        //
        // Handle error
        //
        // console.log( "Error: " + error.what )
    }
```

## Prepare transaction body to send to node

### Create transaction object
```javascript
    var transaction = new Module.Transaction()
```

### Prepare asset transfer
```javascript

     var ret = transaction.Transfer(
            pair,                    // Keys pair
            destination.public_key,  // destination
            "2",                     // block id, @see: getCurrentBlockId bellow
            "0",                     // transaction id, @see: getWalletSate bellow
            0,                       // asset code
            1000.0,                  // amount
            0.0,                     // fee, always 0.0
            "memo field",            // description
            error
     ) 
     if( ret != Module.result.OK ) {
         //
         // Handle error
         //
     }     
     else {
         //
         // call JSON-RPC request the follow format:
         //
         // {
         //      method: "send-transaction",
         //      params: transaction.body,
         //      id: 1,
         //      jsonrpc: "2.0",
         //      version: "1.0"
         //    } 
         //
     }
```

### Prepare emission transaction
```javascript

     var ret = transaction.Emission(
            pair,                    // Keys pair
            "2",                     // block id, @see: getCurrentBlockId bellow
            "0",                     // transaction id, @see: getWalletSate bellow
            0,                       // asset code
            0,                       // fee, always ""
            error
     ) 
     if( ret != Module.result.OK ) {
         //
         // Handle error
         //
     }     
     else {
         //
         // call JSON-RPC request the follow format:
         //
         // {
         //      method: "send-transaction",
         //      params: transaction.body,
         //      id: 1,
         //      jsonrpc: "2.0",
         //      version: "1.0"
         //    } 
         //
     }
```


## JSON-RPC Examples
Assuming we use `axios` package for requests we don't include `import` statement in examples

Json-rpc nodes list available on https://wallet.mile.global/v1/nodes.json
You can use any of node. In examples we use non-existing **example address**: https://example.i.mile.global

### Wallet state
```javascript
getWalletSate( publicKey, count ) {
  return axios.post(
    "https://example.i.mile.global",
    {
      method: "get-wallet-state",
      params: { "public-key": publicKey },
      id: 1,
      jsonrpc: "2.0",
      version: "1.0"
    }
  )
}
//
//  response example: {
//    "id": "1",
//    "jsonrpc": "2.0",
//    "result": {
//      "balance": [ { "amount": "432.67", "code": "0" } ],
//      "exist": "1",
//      "preferred-transaction-id": "16808",
//      "tags": ""
//    },
//    "version": "0.0"
//  }
//
// 
// preferred-transaction-id can be used in transaction builder as a transaction id
```

### Send transaction
```javascript
// transactionData created by transaction.Transfer or transaction.Emission functions above
sendTransaction( transaction_body ) {
  return axios.post(
    "https://example.i.mile.global",
    {
      method: "send-transaction",
      params: transaction_body,
      id: 2,
      jsonrpc: "2.0",
      version: "1.0"
    }
  )
}
```

### Get last transactions
```javascript
getLastTransactions( publicKey, count ) {
  return axios.post(
    "https://example.i.mile.global",
    {
      method: "get-wallet-transactions",
      params: { "public-key": publicKey, "limit": count },
      id: 3,
      jsonrpc: "2.0",
      version: "1.0"
    }
  )
}
```

### Get current block id
```javascript
getCurrentBlockId() {
  return axios.post(
    "https://example.i.mile.global",
    {
      method: "get-current-block-id",
      params: {},
      id: 4,
      jsonrpc: "2.0",
      version: "1.0"
    }
  )
}

//
//  response example: {
//    "id": "4",
//    "jsonrpc": "2.0",
//    "result": { "id": 42 },
//    "version": "1.0"
//  }
//
// 
// id MUST be used in transaction builder as block id
```
