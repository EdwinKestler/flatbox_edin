const Operation = require('../models/OperationModel');

module.exports = (app) => {
    app.get('/operations',async (req,res) => {
        try{
            const results = await Operation.getAll();
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json(results);
            } else {
                res.status(200).json(results);
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

    app.get('/responses',async (req,res) => {
        try{
            const results = await Operation.getResponses();
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json(results);
            } else {
                res.status(200).json(results);
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

    app.get('/operation/:idOperation',async (req,res) => {
        let idOperation = req.params.idOperation
        try{
            const results = await Operation.getOperation(idOperation);
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json(results);
            } else {
                res.status(200).json(results);
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

    app.get('/responses/:idOperation',async (req,res) => {
        let idOperation = req.params.idOperation

        try{
            const results = await Operation.getResponsesById(idOperation);
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json(results);
            } else {
                res.status(200).json(results);
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

    app.get('/operation',async (req,res) => {
        try{
            const results = await Operation.getLastOperation();
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json(results);
            } else {
                res.status(200).json({message: 'Not Content'});
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

    app.post('/operation',async (req,res) => {
        let newOperation = {
            idModule: req.body.idModule,
            gloveCode: req.body.gloveCode,
            number1: req.body.number1,
            operation: req.body.operation,
            number2: req.body.number2,
            result: req.body.result,
            answer: req.body.answer,
            action: req.body.action, 
            topic: req.body.topic
        }

        try{
            const results = await Operation.createOperation(newOperation);
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json({message: 'Pregunta Realizada',operation: results[0][0], status: true});
            } else {
                res.status(200).json({message: 'Not Content'});
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });

    app.post("/response", async (req,res) => {
        let newResponse = {
            idOperation: req.body.idOperation,
            gloveCode: req.body.gloveCode,
            answer: req.body.answer
        }

        try{
            const results = await Operation.createResponse(newResponse);
            if (typeof results !== 'undefined' && results.length > 0) {
                res.status(200).json({message: 'Respuesta Insertada'});
            } else {
                res.status(200).json({message: 'Respuesta Insertada'});
            }
        }catch(error){
            console.log(error);
            res.status(500).json(error);
        }
    });
}

