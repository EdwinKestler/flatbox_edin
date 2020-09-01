const Statistics = require('../models/StatisticsModel');

module.exports = (app) => {
    app.get('/statistics/:idModule',async (req,res) => {
        let idModule = req.params.idModule
        try{
            const results = await Statistics.getModulePoints(idModule);
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

    app.get('/points/:idModule',async (req,res) => {
        let idModule = req.params.idModule
        try{
            const results = await Statistics.getUsersPoints(idModule);
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

    app.get('/module/:idModule',async (req,res) => {
        let idModule = req.params.idModule
        try{
            const results = await Statistics.getAllModule(idModule);
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

    app.get('/date/:idModule',async (req,res) => {
        let idModule = req.params.idModule
        try{
            const results = await Statistics.getDateModule(idModule);
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
}

