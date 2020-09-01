const {pool} = require('../db/connection');
const operationModel = {}

operationModel.getAll = async() => {
    const connection = await pool.getConnection();
    try{
        const data = await connection.query('SELECT * FROM Operations');
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    }
}

operationModel.getOperation = async(idOperation) => {
    const connection = await pool.getConnection();
    try{
        const data = await connection.query(`SELECT * FROM Operations WHERE idOperation = ${idOperation}`);
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    }
}

operationModel.getResponsesById = async(idOperation) => {
    const connection = await pool.getConnection();
    try{
        const data = await connection.query(`SELECT * FROM Responses WHERE idOperation = ${idOperation}`);
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    }
}

operationModel.getResponses = async() => {
    const connection = await pool.getConnection();
    try{
        const data = await connection.query('SELECT * FROM Responses');
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    }
}

operationModel.getLastOperation = async() => {
    const connection = await pool.getConnection();
    try{
        const data = await connection.query('SELECT * FROM Operations WHERE idOperation = (SELECT MAX(idOperation) FROM operations)');
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    } 
}

operationModel.createOperation = async(newOperation) => {

    const connection = await pool.getConnection();
    try{
        const sql = "CALL SP_CreateQuestion(?,?,?,?,?,?,?,?,?)"
        const data = await connection.query(sql,[newOperation.idModule,newOperation.gloveCode,newOperation.number1,newOperation.operation,newOperation.number2,newOperation.result,newOperation.answer,newOperation.action,newOperation.topic]);
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    } 
}

operationModel.createResponse = async(newResponse) => {
    const connection = await pool.getConnection();
    try{
        const sql = "CALL SP_ResponseQuestion(?,?,?)"
        const data = await connection.query(sql,[newResponse.idOperation,newResponse.gloveCode,newResponse.answer]);
        return data[0];
    }catch(ex){
        console.log(ex);
    }finally{
        connection.release();
    } 
}

module.exports = operationModel;