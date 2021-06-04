const express = require('express')
var bodyParser = require('body-parser')
var mysql = require('mysql');

const app = express()


app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: true }))


var con = mysql.createConnection({
  host: "localhost",
  user: "root",
  password: "",
  database: "test"
});

const port = 3000

con.connect(function(err) {
  if (err) throw err;

  console.log("Connected!");
});

app.get('/', (req, res) => {
  res.send('Hello World!').status(200)
})

app.get('/:id', (req, res) => {
  console.log(req.params.id);

  con.query(`SELECT * FROM demotable1 WHERE id = '${req.params.id}'`, function (err, result) {
    if (err) throw err;
    console.log(result);

    res.json( result[0] ).status(200)
  });

})

app.post('/' , (req, res) => {
    console.log( req.body )
    res.send( req.body ).status
})

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
})