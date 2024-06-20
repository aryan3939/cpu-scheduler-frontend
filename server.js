const express = require('express');
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');
const app = express();
const port = 3300;

app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

app.post('/schedule', (req, res) => {

    const { processTableBody, quantumTime } = req.body;

    // Prepare the input for the C++ program
    let input = `${processTableBody.length}\n`;
    processTableBody.forEach(p => {
        input += `${p.arrivalTime} ${p.burstTime} ${p.priority}\n`;
    });
    input += `${quantumTime}\n`;

    console.log("Starting C++ program execution...");

    // Execute the compiled C++ program
    const schedulerPath = path.join(__dirname, 'schedule');
    const child = execFile(schedulerPath, [], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error executing scheduler: ${error}`);
            return res.status(500).send('Error executing scheduler');
        }

        console.log("C++ program executed successfully.");
        res.send(stdout);
    });

    // Provide the input to the C++ program
    child.stdin.write(input);
    child.stdin.end();
    
    const PORT = process.env.PORT || 3300;


    // const { numProcesses, quantumTime, processData } = req.body;

    // // Validate the input
    // if (!numProcesses || !quantumTime || !Array.isArray(processData)) {
    //     return res.status(400).send('Invalid input data');
    // }

    // // Write the process data to a temporary file
    // const tempFilePath = path.join(__dirname, 'temp_process_data.txt');
    // const processDataString = processData.map(process => process.join(' ')).join('\n');
    // const inputString = `${numProcesses}\n${quantumTime}\n${processDataString}`;
    // fs.writeFileSync(tempFilePath, inputString);

    // Execute the C++ binary with the temporary file as input
    // execFile('./scheduler', [], {
    //     input: fs.readFileSync(tempFilePath)
    // }, (error, stdout, stderr) => {
    //     fs.unlinkSync(tempFilePath); // Clean up the temporary file

    //     if (error) {
    //         console.error(`Error executing file: ${error}`);
    //         return res.status(500).send(`Error: ${error.message}`);
    //     }
    //     if (stderr) {
    //         console.error(`Standard error: ${stderr}`);
    //         return res.status(500).send(`Stderr: ${stderr}`);
    //     }

    //     const result = stdout.split('\n').map(line => line.split(' ').map(Number));
    //     res.json(result);
    // });
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}/`);
});



// const express = require('express');
// const bodyParser = require('body-parser');
// const { exec } = require('child_process');

// const app = express();
// const port = 3000;

// // Middleware to parse form data
// app.use(bodyParser.urlencoded({ extended: true }));
// app.use(bodyParser.json());

// // Serve static files from the 'public' directory
// app.use(express.static('public'));

// // Endpoint to handle form submissions
// app.post('/submit', (req, res) => {
//     const numProcesses = req.body.numProcesses;
//     const quantumTime = req.body.quantumTime;
//     const processes = req.body.processes;

//     // Prepare input for the C++ program
//     let input = `${numProcesses} ${quantumTime}\n`;
//     processes.forEach(process => {
//         input += `${process.id} ${process.arrivalTime} ${process.burstTime} ${process.priority}\n`;
//     });

//     // Call the C++ program
//     const cppProcess = exec('./scheduler', (error, stdout, stderr) => {
//         if (error) {
//             console.error(`exec error: ${error}`);
//             return res.status(500).send('Server error');
//         }

//         // Send the output back to the client
//         res.send(stdout);
//     });

//     // Write input to the C++ program
//     cppProcess.stdin.write(input);
//     cppProcess.stdin.end();
// });

// app.listen(port, () => {
//     console.log(`Server is running on http://localhost:${port}`);
// });