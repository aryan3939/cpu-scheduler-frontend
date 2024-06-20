const serverURL = 'http://localhost:3300';

document.addEventListener('DOMContentLoaded', () => {
    const numProcessesInput = document.getElementById('numProcesses');
    const quantumTimeInput = document.getElementById('quantumTime');
    const randomDataBtn = document.getElementById('randomDataBtn');
    const compareBtn = document.getElementById('compareBtn');
    const resetBtn = document.getElementById('resetBtn');
    const processTableBody = document.getElementById('processTableBody');
    const visualizationSection = document.getElementById('visualizationSection');
    const outputTableBody = document.getElementById('outputTableBody');

    numProcessesInput.addEventListener('input', createRows);
    randomDataBtn.addEventListener('click', fillRandomData);
    compareBtn.addEventListener('click', compareAlgorithms);
    resetBtn.addEventListener('click', resetForm);

    function createRows() {
        const numProcesses = parseInt(numProcessesInput.value);
        processTableBody.innerHTML = '';

        if (isNaN(numProcesses) || numProcesses <= 0) {
            return;
        }

        for (let i = 0; i < numProcesses; i++) {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td id="id_no">P${i + 1}</td>
                <td><input id="table_no" type="number" min="0" max="10" class="arrivalTime" placeholder="Enter data"></td>
                <td><input id="table_no" type="number" min="1" max="10" class="burstTime" placeholder="Enter data"></td>
                <td><input id="table_no" type="number" min="1" max="${numProcesses}" class="priority" placeholder="Enter data"></td>
            `;
            processTableBody.appendChild(row);
        }
    }

    function fillRandomData() {
        let numProcesses = parseInt(numProcessesInput.value);
        createRows();

        if (isNaN(numProcesses) || numProcesses <= 0) {
            numProcesses = getRandomInt(2, 10);
            numProcessesInput.value = numProcesses;
            createRows();
        }

        let totalBurstTime = 0;

        document.querySelectorAll('#processTableBody tr').forEach((row, index) => {
            const burstTime = getRandomInt(1, 10);
            const priority = getRandomInt(1, numProcesses);

            row.querySelector('.burstTime').value = burstTime;
            row.querySelector('.priority').value = priority;

            totalBurstTime += burstTime;
        });

        if (isNaN(quantumTimeInput.value) || quantumTimeInput.value <= 0) {
            quantumTimeInput.value = Math.ceil((totalBurstTime / (2 * numProcesses)) + 1);
        }

        let variable = 0;
        document.querySelectorAll('#processTableBody tr').forEach((row, index) => {
            const arrivalTime = index === 0 ? 0 : (variable += getRandomInt(0, 6));
            row.querySelector('.arrivalTime').value = arrivalTime;
        });
    }

    function compareAlgorithms() {
        const processes = [];
        const quantumTime = parseInt(quantumTimeInput.value);

        let allFieldsFilled = true;

        document.querySelectorAll('#processTableBody tr').forEach((row, index) => {
            const id = `P${index + 1}`;
            const arrivalTime = parseInt(row.querySelector('.arrivalTime').value);
            const burstTime = parseInt(row.querySelector('.burstTime').value);
            const priority = parseInt(row.querySelector('.priority').value);
            if (isNaN(arrivalTime) || isNaN(burstTime) || isNaN(priority)) {
                allFieldsFilled = false;
                return;
            }
            processes.push({ id, arrivalTime, burstTime, priority });
        });

        if (!allFieldsFilled) {
            alert('Please fill in all the fields.');
            return;
        }

        const requestData = {
            processTableBody: processes,
            quantumTime: quantumTime
        };

        fetch(`http://localhost:3300/schedule`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestData)
        })
        .then(response => response.json())
        .then(data => {
            visualizeData(data);
        })
        .catch(error => {
            console.error('Error:', error);
        });

        visualizationSection.style.display = 'block';
        randomDataBtn.disabled = true;
    }

    function resetForm() {
        numProcessesInput.value = '';
        quantumTimeInput.value = '';
        processTableBody.innerHTML = `
            <tr>
                <td id="id_no">P1</td>
                <td><input id="table_no" type="number" min="0" class="arrivalTime" placeholder="Enter data"></td>
                <td><input id="table_no" type="number" min="1" class="burstTime" placeholder="Enter data"></td>
                <td><input id="table_no" type="number" min="1" class="priority" placeholder="Enter data"></td>
            </tr>
        `;
        visualizationSection.style.display = 'none';
        outputTableBody.innerHTML = '';
        randomDataBtn.disabled = false;
    }

    function getRandomInt(min, max) {
        return Math.floor(Math.random() * (max - min + 1)) + min;
    }

    function visualizeData(data) {
        outputTableBody.innerHTML = ''; // Clear the output table

        data.processes.forEach((process, index) => {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${process.id}</td>
                <td>${process.arrivalTime}</td>
                <td>${process.burstTime}</td>
                <td>${process.priority}</td>
                <td>${process.completionTime}</td>
                <td>${process.turnaroundTime}</td>
                <td>${process.waitingTime}</td>
            `;
            outputTableBody.appendChild(row);
        });

        // Log the additional data for Gantt chart and bar charts
        console.log('Gantt Chart Data:', data.ganttChart);
        console.log('Average Waiting Time:', data.averageWaitingTime);
        console.log('Average Turnaround Time:', data.averageTurnaroundTime);
        console.log('CPU Utilization:', data.cpuUtilization);

        renderGanttChart(data.ganttChart);
        renderBarChart('barChart1', 'Average Waiting Time', data.averageWaitingTime);
        renderBarChart('barChart2', 'Average Turnaround Time', data.averageTurnaroundTime);
        renderBarChart('barChart3', 'CPU Utilization', data.cpuUtilization);
    }

    function renderGanttChart(data) {
        // Implement Gantt chart rendering here using a library like Chart.js
    }

    function renderBarChart(chartId, label, data) {
        // Implement bar chart rendering here using a library like Chart.js
    }
});
