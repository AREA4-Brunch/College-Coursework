// const START_GAME_BTN = document.querySelector("#start-game button");
const START_GAME_BTN = $("#start-game button")[0];


// ====================================
// Event Listeners:


window.addEventListener("load", function() {
    // TODO: remove
    // setTestVars();

    fadeOutPageLoader();
});


/**
 *  Config the game settings and go to page
 *  with the game.
 */
START_GAME_BTN.addEventListener("click", () => {
    setGameSettings();
    window.location.href = "snake_game.html";
})


// ====================================


/**
 * Sets the animation on the page loader and waits for it to end.
 */
async function fadeOutPageLoader() {
    const page_loader = document.getElementById("page-loader-wrapper");

    await new Promise((resolve) => {
        page_loader.addEventListener("animationend", () => {
            document.body.removeChild(page_loader);
            resolve();
        });

        page_loader.classList.add("fade-out");
    });
}

/**
 * Sets game settings form data into local storage.
 */
function setGameSettings() {
    localStorage.setItem(
        "velocity",
        getSelectedVelocity().toString()
    );

    localStorage.setItem(
        "board-dims",
        JSON.stringify(getSelectedBoardDimensions())
    );
}

function getSelectedVelocity() {
    // const velocity = document.querySelector("#game-difficulty input[type='radio']:checked")
    //                          .value;
    const velocity = parseInt(
        $("#game-difficulty input[type='radio']:checked").val()
    );

    return velocity;
}

/**
 * 
 * @returns 2 el list = [ rows:int, cols:int ]
 */
function getSelectedBoardDimensions() {
    // const selected_val = document.querySelector("#board-size option[selected]")
    //                               .value;
    const selected_val = $("#board-size").val();
    const rows = parseInt(selected_val);
    const cols = parseInt(selected_val);
    return [ rows, cols ];
}


// NOT USED:
function clearLocalStorageVars() {
    localStorage.removeItem("username");
    localStorage.removeItem("last-user-score");
    localStorage.removeItem("velocity");
    localStorage.removeItem("board-dims");
    localStorage.removeItem("leaderboard");
}


function setTestVars() {
    // Test vars:
    localStorage.setItem("username", "Jim");
    localStorage.setItem("last-user-score", "7500");

    const leaderboard_data = [
        [ "Jim", 12000 ],
        [ "Morrison", 10000 ],
        [ "God@27", 8000 ],
        [ "God@27", 8000 ],
        [ "God@27", 8000 ],
    ];

    localStorage.setItem(
        "leaderboard", 
        JSON.stringify(leaderboard_data)
    );

    console.log(`username: ${localStorage.getItem("username")}`);
    console.log(`last-user-score: ${localStorage.getItem("last-user-score")}`);
    console.log(`velocity: ${localStorage.getItem("velocity")}`);
    console.log(`board-dims: ${localStorage.getItem("board-dims")}`);
    console.log(`leaderboard: ${localStorage.getItem("leaderboard")}`);
    // End of Test vars:
}
