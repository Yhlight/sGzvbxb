console.log('This script was imported from test_script.js');

document.addEventListener('DOMContentLoaded', function() {
    console.log('Imported script is running!');
    const content = document.querySelector('.imported-content');
    if (content) {
        content.addEventListener('click', function() {
            alert('You clicked the imported content!');
        });
    }
});