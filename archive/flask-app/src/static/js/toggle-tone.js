document.getElementById('toggle-tone').addEventListener('click', function() {
    const toggleButton = document.getElementById('toggle-tone');

    if (toggleButton.classList.contains('active')) {
        // If the button is in "active" state, stop the tone
        stopTone();
        toggleButton.textContent = 'Start Tone';
        toggleButton.classList.remove('active');
    } else {
        // If the button is not active, start the tone
        startTone(440); // You can change the frequency as needed
        toggleButton.textContent = 'Stop Tone';
        toggleButton.classList.add('active');
    }
});

function startTone(frequency) {
    fetch('/play-tone', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ frequency: frequency })
    })
    .then(response => response.json())
    // .then(data => {
    //     alert(data.message || data.error);
    // });
}

function stopTone() {
    fetch('/stop-tone', {
        method: 'POST'
    })
    .then(response => response.json())
    // .then(data => {
    //     alert(data.message || data.error);
    // });
}
