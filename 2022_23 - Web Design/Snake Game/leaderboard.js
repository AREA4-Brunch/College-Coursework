
// ====================================
// Event Listeners:


document.addEventListener("DOMContentLoaded", () => {
    updateUserLastScore();
    updateLeaderboard();
});


window.addEventListener("load", function() {
    fadeOutPageLoader();
});


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
 * leaderboard_data is read from localStorage and
 * expected in format, arr size is not checked,
 * arr is not sorted; only displayed entirely:
 * [
 *  [ username: string, score: string ]
 * ]
 */
function updateLeaderboard() {
    const leaderboard_data = JSON.parse(localStorage.getItem("leaderboard"));
    if (leaderboard_data === null) {  // no users played yet
        leaderboard_data = [];
    }

    // add the leaderboard data to the table
    let new_tbody = document.createElement("tbody");

    for (let i = 0; i < leaderboard_data.length; ++i) {
        let tr = document.createElement("tr");

        let ordinal = document.createElement("th");
        ordinal.setAttribute("scope", "row");
        ordinal.textContent = i + 1;
        ordinal.classList.add("center-horizontally");

        let username = document.createElement("td");
        username.textContent = leaderboard_data[i][0];

        let score = document.createElement("td");
        score.textContent = leaderboard_data[i][1];

        tr.appendChild(ordinal);
        tr.appendChild(username);
        tr.appendChild(score);

        new_tbody.appendChild(tr);
    }

    // const cur_body = document.querySelector("#leaderboard tbody");
    const cur_body = $("#leaderboard tbody")[0];
    cur_body.parentNode.replaceChild(
        new_tbody,
        // fetch current tbody to be replaced
        cur_body
    );
}


/**
 * user's last game's score is read from localStorage and
 * expected items in localStorage are:
 *      username: string
 *      last-user-score: int
 */
function updateUserLastScore() {
    const username = localStorage.getItem("username");

    // if last user game was not recorded then
    // ignore the section
    if (username === null) {
        document.getElementById("last-score")
                .style.visibility = "hidden";
        return;
    }

    // update username:
    document.getElementById("username")
            .textContent = username;

    // update last score:
    const last_user_score = parseInt(localStorage.getItem("last-user-score"));
    document.getElementById("last-user-score")
            .textContent = last_user_score;
}
