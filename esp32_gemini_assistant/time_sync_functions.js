// Time sync functions to add to the web interface
function syncTime() {
    const timeInput = document.getElementById('currentTime').value;
    if(!timeInput) {
        alert('Please select a time');
        return;
    }
    
    const date = new Date(timeInput);
    const timestamp = Math.floor(date.getTime() / 1000);
    
    fetch('/api/time', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({timestamp: timestamp})
    }).then(() => {
        alert('Time synchronized successfully!');
        document.getElementById('currentTime').value = '';
    });
}

function syncWithNTP() {
    fetch('/api/time/ntp', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if(data.success) {
                alert('Time synchronized with NTP server!');
            } else {
                alert('NTP sync failed: ' + data.error);
            }
        });
}
