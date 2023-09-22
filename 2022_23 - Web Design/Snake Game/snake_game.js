// =========================================
// Globals:


let GAME = null;


// =========================================
// DOM Globals:


// let SAVE_SCORE_MODAL = null;


// =========================================
// Event Listeners and DOM Initialisations:


document.addEventListener("DOMContentLoaded", () => {
    validateLocalStorageVariables();

    // init the game
    GAME = new SnakeGame(...getGameSettings());

    // init the DOM:
    initSaveScoreModal();
    initCurrentUserScoreLabel();

    // TODO: remove
    // console.log(`username: ${localStorage.getItem("username")}`);
    // console.log(`last-user-score: ${parseInt(localStorage.getItem("last-user-score"))}`);
    // console.log(`velocity: ${parseInt(localStorage.getItem("velocity"))}`);
    // console.log(`board-dims: ${JSON.parse(localStorage.getItem("board-dims"))}`);
    // console.log(`leaderboard: ${JSON.parse(localStorage.getItem("leaderboard"))}`);
});


window.addEventListener("load", function() {
    fadeOutPageLoader();
    // GAME.init();  // setup arrow keys listeners, etc.
    GAME.startGame();
});


function initSaveScoreModal() {
    const save_score_modal_dom = $("#save-score")[0];
    const save_score_modal_bootstrap = new bootstrap.Modal(save_score_modal_dom, {
        keyboard: false  // disable ESC key closing the modal
    });

    // when game is over display the modal for user to save their score
    save_score_modal_dom.addEventListener(SnakeGame.Event.ON_GAME_OVER, (e) => {
        // show the score to offer user to save:
        $("#save-score-label")[0].textContent = `Save Score: ${e.detail.score}`;

        // suggest last username whose score was stored if any,
        // else show basic placeholder value
        let save_score_username = $("#save-score-username")[0];
        const username = getLastUsername();
        save_score_username.placeholder = "YOUR HANDLE";
        if (username !== null) {
            save_score_username.value = username;
        }

        // show the modal for saving score under username
        save_score_modal_bootstrap.toggle();
    });

    // register the modal with the game
    GAME.addEventSubscriber(
        save_score_modal_dom,
        SnakeGame.Event.ON_GAME_OVER
    );

    // on enter submit the username under which the current
    // score should be stored
    const username_input = document.getElementById("save-score-username");
    username_input.addEventListener("keypress", (e) => {
        if (e.key !== "Enter") { return; }

        if ( username_input.value === undefined
          || username_input.value === ""
        ) {
            return;
        }

        // fetch the username user entered and store it with score
        const username = username_input.value;
        const user_score = GAME.score
        updateLastUsername(username);
        updateLastUserScore(user_score);
        updateLeaderboard(username, user_score);

        // hide the modal for saving score under username
        save_score_modal_bootstrap.toggle();

        // switch to the leaderboard page
        window.location.href = "leaderboard.html";
    });
}

function initCurrentUserScoreLabel() {
    const cur_score_label = document.getElementById('cur-user-score');

    cur_score_label.addEventListener(SnakeGame.Event.ON_SCORE_CHANGE, (e) => {
        cur_score_label.innerHTML = e.detail.score.toString();
    });

    // register the label with the game
    GAME.addEventSubscriber(
        cur_score_label,
        SnakeGame.Event.ON_SCORE_CHANGE
    );

}


// =========================================
// Utilities:


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


// =========================================
// Managing Local Storage:


function validateLocalStorageVariables() {
    try {
        const velocity = parseInt(localStorage.getItem("velocity"));
        if (velocity === null) { throw "Velocity was not set"; }

        const board_dims = JSON.parse(localStorage.getItem("board-dims"));
        if (board_dims === null) { throw "Board dims was not set"; }

    } catch (err) {
        // TODO: instead of alert use confirm and redirect if OK
        //      else just rethrow so the domcontent loaded and 
        //      window load do nothing

        // if some variable was not set, alert and redirect to home page:
        if (err.includes("was not set")) {
            alert( "Necessary cookies were not set. "
                 + "Try starting the game from the home page or enable cookies for this website."
            );

           window.location.href = "./index.html";
        }
    }
}


/**
 * 
 * @returns [ velocity: int, num_rows: int, num_cols: int ]
 */
function getGameSettings() {
    const velocity = parseInt(localStorage.getItem("velocity"));
    const board_dims = JSON.parse(localStorage.getItem("board-dims"));

    return [ velocity, ...board_dims ];
}


function getLeaderboardData() {
    return JSON.parse(localStorage.getItem("leaderboard"));
}


function getLastUsername() {
    return localStorage.getItem("username");
}


// NOT USED
function getLastUserScore() {
    return localStorage.getItem("last-user-score");
}


function updateLastUsername(username) {
    localStorage.setItem("username", username);
}


/**
 * 
 * @param {int} user_score
 */
function updateLastUserScore(user_score) {
    localStorage.setItem("last-user-score", user_score.toString());
}


function updateLeaderboard(username, score) {
    let leaderboard = getLeaderboardData();
    if (leaderboard === null) {
        leaderboard = new Array();
    }

    leaderboard.push([ username, score ]);
    leaderboard.sort((x, y) => {
        return parseInt(y[1]) - parseInt(x[1]);
    });

    // keep only top 5
    while (leaderboard.length > 5) {
        leaderboard.pop();
    }

    localStorage.setItem(
        "leaderboard",
        JSON.stringify(leaderboard)
    );
}


// =========================================
// Game:


class SnakeGame {

    static GameOverException = class Exception extends Error {
        constructor(game_over_cause) {
            super(game_over_cause);
        }
    };

    static CellIndexOutOfBounds = class Exception extends Error {
        constructor(msg) {
            super(msg);
        }
    };

    // enum for game status
    static Status = Object.freeze({
        LOADING:  0,
        READY:    1,
        PAUSED:   2,
        PLAYING:  3,
        OVER:     4
    });

    // enum for game events that can be listened to,
    // push config
    static Event = Object.freeze({
        ON_GAME_START:  "gamestart",
        ON_GAME_OVER:   "gameover",
        ON_STATUS_CHANGE:  "statuschange",
        ON_SCORE_CHANGE:  "scorechange"
    });

    #status = SnakeGame.Status.LOADING;
    #num_rows = 0;
    #num_cols = 0;
    #score = -1;

    // subscribers to event e.g. on game over
    #event_subscribers = new Map();
    #user_ctrls_event_listeners = new Array();
    #super_food_generator = null;
    #snake = null;
    #initial_velocity = -1;

    // =================================
    // Initialization:

    constructor(initial_velocity, num_rows, num_cols) {
        this.#initial_velocity = initial_velocity;
        this.#num_rows = num_rows;
        this.#num_cols = num_cols;

        this.#score = -1;  // !important to be != 0 before startGame for 1st time
        this.#setStatus(SnakeGame.Status.LOADING);
        this.#event_subscribers = new Map();
        this.#user_ctrls_event_listeners = new Array();
        this.#super_food_generator = new SuperFoodGenerator(this, 10000, 4000);
        this.#snake = new Snake(this);
        this.#constructBoard();
    }

    /**
     * For reusable setup/construction of object,
     * sets listeners for keyboard game controls
     */
    #init() {
        this.#initUserControls();
        this.#initBoard()
        this.#snake.init(this.#initial_velocity, 0, 1);
        this.#setScore(0);
        this.#setStatus(SnakeGame.Status.READY);
    }

    // =================================
    // Initialization Helpers:

    #constructBoard() {
        let board = document.getElementById("board");
        board.innerHTML = "";

        let board_body = document.createElement("tbody");

        for (let row = 0; row < this.#num_rows; ++row) {
            let tr = document.createElement("tr");

            for (let col = 0; col < this.#num_cols; ++col) {
                let cell = document.createElement("td");
                cell.style.backgroundColor = (row + col) % 2 === 0 ?
                                              "whitesmoke"
                                            : "rgb(153, 192, 250)";
                let cell_content = document.createElement("div");
                cell_content.classList.add("cell-content");
                cell.appendChild(cell_content);
                tr.appendChild(cell);
            }

            board_body.appendChild(tr);
        }

        board.appendChild(board_body);
    }

    #initBoard() {
        $('#board .cell-content').each((index, element) => {
            element.innerHTML = '';
        });
    }

    #initUserControls() {
        // generate wrappers for listener functions so `this`
        // can be passed
        const arrow_key_controls = (e) => {
            this.#arrowKeyControls(e);
        };

        // store the even listeners so they can be removed later
        this.#user_ctrls_event_listeners.push(["keyup", arrow_key_controls]);

        // add all stored user controls' event listeners
        for (const e_listener of this.#user_ctrls_event_listeners) {
            document.addEventListener(...e_listener);
        }
    }

    #disableUserControls() {
        while (this.#user_ctrls_event_listeners.length !== 0) {
            const e_listener = this.#user_ctrls_event_listeners.pop();
            document.removeEventListener(e_listener[0], e_listener[1]);
        }
    }

    #arrowKeyControls(e) {
        switch (e.key) {
            case "ArrowUp":
                this.#snake.setNextDirectionChange(-1 * this.#num_cols);
                break;

            case "ArrowDown":
                this.#snake.setNextDirectionChange(this.#num_cols);
                break;

            case "ArrowLeft":
                this.#snake.setNextDirectionChange(-1);
                break;

            case "ArrowRight":
                this.#snake.setNextDirectionChange(1);
                break;

            default:  // unrecognized key
                break;
        }
    }

    // =================================
    // Public Methods:

    startGame() {
        this.#init();
        this.generateNormalFood();
        this.#super_food_generator.start();
        this.#snake.startMovement();
        this.#setStatus(SnakeGame.Status.PLAYING);
        this.#notifyOnGameStart();
    }

    play() {
        if (this.#status !== SnakeGame.Status.PAUSED) {
            return;
        }

        // TODO: stuff

        this.#setStatus(SnakeGame.Status.PLAYING);
    }

    pause() {
        if (this.#status !== SnakeGame.Status.PLAYING) {
            return;
        }

        // TODO: stuff

        this.#setStatus(SnakeGame.Status.PAUSED);
    }

    addEventSubscriber(subscriber, event_name) {
        let event_group = this.#event_subscribers.get(event_name);

        // new event group
        if (event_group === undefined) {
            this.#event_subscribers.set(
                event_name,
                Array.from([ subscriber ])
            );
        } else {  // add to existing event group
            event_group.add(subscriber);
        }
    }

    /**
     * 
     * @returns random available cell idx if any,
     *          else -1
     */
    getRandomAvailableCellIndex() {
        let available_indices = [];

        $('#board .cell-content').each((index, element) => {
            if (!element.hasChildNodes() && element.textContent === '') {
                available_indices.push(index);
            }
        });

        // if no available cells return -1
        if (available_indices.length === 0) {
            return -1;
        }

        const rnd_avail_cell_idx = Math.floor(Math.random() * available_indices.length);
        return available_indices[rnd_avail_cell_idx];
    }

    /**
     * 
     * @param {int} cell_idx Cell index into which given object is added.
     * @param {Element} dom_object DOM object to add into the cell.
     * @returns Cell DOM object into which given object was added.
     */
    insertIntoCell(cell_idx, dom_object) {
        if (this.isCellIdxOutOfBounds(cell_idx)) {
            throw new SnakeGame.CellIndexOutOfBounds('');
        }

        let cell = $('#board .cell-content').get(cell_idx);
        cell.appendChild(dom_object);
        return cell;
    }

    isCellIdxOutOfBounds(idx) {
        if (idx < 0) return true;
        const [row, col] = this.getCellCoordinates(idx);
    
        if (row < 0 || row >= this.#num_rows) {
            return true;
        }

        if (col < 0 || col >= this.#num_cols) {
            return true;
        }

        return false;
    }

    /**
     * 
     * @param {int} cell_idx 
     * @returns Whether any food was collected.
     */
    eatFood(cell_idx) {
        if (this.isCellIdxOutOfBounds(cell_idx)) {
            throw new SnakeGame.CellIndexOutOfBounds('');
        }

        const found_food = $('#board .cell-content').eq(cell_idx).find('.food').get();
        if (found_food.length === 0) { return false; }

        // add points for the food
        let food_value = 0;
        // let ate_normal_food = false;
        for (const food of found_food) {
            if (food.classList.contains('normal-food')) {
                // ate_normal_food = true;
                food_value += NormalFood.VALUE;
                // food.parentNode.removeChild(food);
            } else if (food.classList.contains('super-food')) {
                food_value += SuperFood.VALUE;
                // food.parentNode.removeChild(food);
            }
        }

        // TODO: fixes a bug when snake eats the food but both food
        //      and head are in the same cell, find more elegant solution
        found_food[0].parentNode.innerHTML = '';

        this.#setScore(this.#score + food_value);
        // if (ate_normal_food) {
        //     this.generateNormalFood();
        // }
        return true;
    }

    containsClass(cell_idx, class_name) {
        const found = $('#board .cell-content').eq(cell_idx).find('.' + class_name).get();
        return found.length !== 0;
    }

    // =================================
    // Getters:

    *getDimensions() {
        yield this.#num_rows;
        yield this.#num_cols;
    }

    getCellCoordinates(cell_idx) {
        const row = Math.floor(cell_idx / this.#num_rows);
        const col = cell_idx - row * this.#num_rows;
        return [ row, col ];
    }

    get status() {
        return this.#status;
    }

    get score() {
        return this.#score;
    }

    // =================================
    // Setters:

    #setStatus(status) {
        if (this.#status === status) return;

        this.#status = status;
        this.#notifyOnStatusChange();
    }

    #setScore(score) {
        if (this.#score === score) return;

        this.#score = score;
        this.#notifyOnScoreChange();
    }

    // =================================
    // Private Game Mechanics Methods:

    gameOver() {
        this.#snake.stopMovement();
        this.#disableUserControls();
        this.#super_food_generator.stop();
        this.#setStatus(SnakeGame.Status.OVER);
        this.#notifyOnGameOver();
    }

    generateNormalFood() {
        // limit to only 1 normal food on board
        if ($('#board .normal-food').get().length !== 0) {
            return;
        }

        let cell_idx = this.getRandomAvailableCellIndex();
        // if no space for the food do not add it
        if (cell_idx < 0) return;
        this.insertIntoCell(cell_idx, new NormalFood().asDOM());
    }

    // =================================
    // Private Subscribe/Notify Methods:

    #notifyOnGameStart() {
        const event = new CustomEvent(SnakeGame.Event.ON_GAME_START);
        this.#notifyEventGroup(event);
    }

    #notifyOnGameOver() {
        // push the new final score
        this.#notifyEventGroup(new CustomEvent(
            SnakeGame.Event.ON_GAME_OVER,
            {
                detail: {
                    score: this.#score
                }
            }
        ));
    }

    #notifyOnStatusChange() {
        // push the new status
        this.#notifyEventGroup(new CustomEvent(
            SnakeGame.Event.ON_STATUS_CHANGE,
            {
                detail: {
                    status: this.#status
                }
            }
        ));
    }

    #notifyOnScoreChange() {
        // push the new status
        this.#notifyEventGroup(new CustomEvent(
            SnakeGame.Event.ON_SCORE_CHANGE,
            {
                detail: {
                    score: this.#score
                }
            }
        ));
    }

    #notifyEventGroup(event_notification) {
        let event_group = this.#event_subscribers
                              .get(event_notification.type);

        if (event_group === undefined) return;  // no subs

        event_group.forEach(subscriber => {
            subscriber.dispatchEvent(event_notification);
        });
    }

}


class Food {

    #value = 0;

    constructor(points) {
        if (this.constructor == Food) {
            throw new Error("Food is an abstract class");
        }

        this.#value = points;
    }

    get value() {
        return this.#value;
    }

    asHTML() {
        return this.asDOM().innerHTML;
    }

    asDOM() {
        let food = document.createElement('div');
        food.classList.add('food');
        return food;
    }

}


class NormalFood extends Food {

    static VALUE = 1;

    constructor() {
        super(NormalFood.VALUE);
    }

    asDOM() {
        let food = super.asDOM();
        food.classList.add('normal-food');
        return food;
    }

}


class SuperFood extends Food {

    static VALUE = 10;

    constructor() {
        super(SuperFood.VALUE);
    }

    asDOM() {
        let food = super.asDOM();
        food.classList.add('super-food');
        return food;
    }

}


class SuperFoodGenerator {

    #gen_super_food_time_interval_id = -1;
    #generation_freq_ms = 10000;
    #food_ttl_ms = 3500;  // must be less than `generation_freq_ms`
    #snake_game;

    /**
     * 
     * @param {int} generation_freq_ms Time interval in ms for food to be generated
     * @param {int} food_ttl_ms Time interval in ms of generated food being removed
     *                          if it has not been already.
     */
    constructor(snake_game, generation_freq_ms, food_ttl_ms) {
        this.#snake_game = snake_game;
        this.#generation_freq_ms = generation_freq_ms;
        this.#food_ttl_ms = food_ttl_ms;
    }

    start() {
        // destroy existing time interval
        if (this.#gen_super_food_time_interval_id != -1) {
            this.stopSuperFoodGenerator();
        }

        // generate super food every 10 seconds
        this.#gen_super_food_time_interval_id = setInterval(() => {
            // if there was already any super food remove it
            this.#removeAllSuperFood();
            this.#generateSuperFood();
        }, this.#generation_freq_ms);
    }

    stop() {
        if (this.#gen_super_food_time_interval_id == -1) {
            return;
        }

        try {
            clearInterval(this.#gen_super_food_time_interval_id);
        } catch (error) {
            console.log(`Failed to stop super food gen, clear the interval with id:
                        ${this.#gen_super_food_time_interval_id} with caught error:\n ${error.message}`);
        }
    }

    #removeAllSuperFood() {
        $('#board .super-food').each((index, element) => {
            element.parentNode.innerHTML = '';
        });
    }

    #generateSuperFood() {
        const cell_idx = this.#snake_game.getRandomAvailableCellIndex();
        // if no space for the food do not add it
        if (cell_idx < 0) return;
        const food = new SuperFood().asDOM();
        const cell = this.#snake_game.insertIntoCell(cell_idx, food);

        // clear the super food off the board after 3.5 secs
        // in case it gets eaten before those 3.5 secs new one
        // is assumed not to be generated at the same cell before
        // those 3.5 secs expire
        setTimeout(() => {
            if ($('#board .cell-content').get(cell_idx).contains(food)) {
                cell.removeChild(food);
            }
        }, this.#food_ttl_ms);
    }

}


class Snake {

    #snake_game = null;
    // body contains DOM objects in the table cells
    // that sequentially represent the body of the snake
    #head_cell_idx = -1;  // cell_idx of the head
    #body = new Array();  // including the head
    // the increment to be added to head_cell_idx at end
    // of each move: 1 right, -1 left, -num_cols up, num_cols down
    #direction_change = 1;
    #next_direction_change = 1;
    #velocity = 0;  // freq in ms, at which the move() will be called
    #movement_interval_id = -1;

    /**
     * 
     * @param {*} direction_change 
     * @param {int} initial_position Initial cell idx of the head.
     */
    constructor(snake_game) {
        this.#snake_game = snake_game;
    }

    setNextDirectionChange(next_direction_change) {
        // cannot set the opposite direction of the one it is going in
        // if the snake consiste of more than just the head
        if (!this.#canSetNextDirection(next_direction_change)) {
            return;
        }

        this.#next_direction_change = next_direction_change;
        this.#move();
    }

    init(velocity, initial_position, direction_change) {
        this.#velocity = velocity;
        this.#direction_change = direction_change;
        this.#next_direction_change = this.#direction_change;

        // add the head
        this.#head_cell_idx = initial_position;
        this.#body = new Array();
        this.#body.push(this.#headAsDOM());
        this.#snake_game.insertIntoCell(this.#head_cell_idx, this.#body[0]);
    }

    startMovement() {
        // destroy existing time interval
        if (this.#movement_interval_id != -1) {
            this.stopMovement();
        }

        // move every velocity milliseconds
        this.#movement_interval_id = setInterval(() => {
            this.#move();
        }, this.#velocity);
    }

    stopMovement() {
        if (this.#movement_interval_id == -1) {
            return;
        }

        try {
            clearInterval(this.#movement_interval_id);
        } catch (error) {
            console.log(`Failed to stop movement, clear the interval with id:
                        ${this.#movement_interval_id} with caught error:\n ${error.message}`);
        }
    }

    #move() {
        try {
            this.#direction_change = this.#next_direction_change;
            const next_head_cell_idx = this.#nextHeadPosition();

            if (this.#willHitWall(next_head_cell_idx)) {
                throw new SnakeGame.GameOverException('Snake hit the wall.');
            } else if (this.#snake_game.isCellIdxOutOfBounds(next_head_cell_idx)) {
                // if moved out of the bounds of the board then it went
                // throguh the wall
                throw new SnakeGame.GameOverException('Snake hit the wall.');
            }

            // move the body by removing the tail if
            // no food was eaten during this movemenet, else
            // keep the snake's size => it grew this move
            let has_eaten_food = false;
            if (!this.#snake_game.eatFood(next_head_cell_idx)) {
                has_eaten_food = true;
                // remove the tail
                const tail = this.#body.pop();
                tail.parentNode.removeChild(tail);
            }
            // else do not shrink the tail => snake grows

            // having moved the tail first, move the head in
            // the set direction if it is a valid move,
            // else throw game over
            const old_head_cell_idx = this.#head_cell_idx;
            // !important must be set after the tail was removed
            this.#setHeadPosition(next_head_cell_idx);  // throws if invalid
            if (this.#body.length > 0) {  // update the prev head to non-head style
                this.#body[0].parentNode.removeChild(this.#body[0]);
                this.#body[0] = this.#bodyPartAsDOM();
                this.#snake_game.insertIntoCell(old_head_cell_idx, this.#body[0]);
            }
            this.#body.unshift(this.#headAsDOM());
            this.#snake_game.insertIntoCell(this.#head_cell_idx, this.#body[0]);

            // generate normal food after eating any kind food as there could
            // have been only super food on the board due to lack of space
            if (has_eaten_food) {
                this.#snake_game.generateNormalFood();
            }

        } catch (error) {
            this.#snake_game.gameOver();
            console.log(`Game over: ${error.message}`);
        }
    }

    #headAsDOM() {
        let head = this.#bodyPartAsDOM();
        head.classList.add('snake-head');
        return head;
    }

    #bodyPartAsDOM() {
        let part = document.createElement('div');
        part.classList.add('snake');
        return part;
    }

    #nextHeadPosition() {
        return this.#head_cell_idx + this.#direction_change;
    }

    #setHeadPosition(next_head_cell_idx) {
        if (this.#snake_game.isCellIdxOutOfBounds(next_head_cell_idx)) {
            throw new SnakeGame.GameOverException('Snake hit the wall.');
        }

        if (this.#snake_game.containsClass(next_head_cell_idx, 'snake')) {
            throw new SnakeGame.GameOverException('Snake bit itself.');
        }

        this.#head_cell_idx = next_head_cell_idx;
    }

    #canSetNextDirection(next_direction_change) {
        if (this.#body.length <= 1) { return true; }

        // const next_head_cell_idx = this.#head_cell_idx + next_direction_change;
        // let user set the command to hit the wall and end the game
        // if (this.#willHitWall(next_head_cell_idx)) { return true; }
        // return !this.#snake_game.containsClass(next_head_cell_idx, 'snake');

        if (next_direction_change === -1 * this.#direction_change) {
            return false;
        }

        return true;
    }

    #willHitWall(next_head_cell_idx) {
        const [cur_row, cur_col] = this.#snake_game.getCellCoordinates(this.#head_cell_idx);
        const [next_row, next_col] = this.#snake_game.getCellCoordinates(next_head_cell_idx);

        // if moved left/right and ended up in different row then
        // it went through the wall
        if (Math.abs(this.#direction_change) === 1) {
            if (cur_row !== next_row) {
                return true;
            }
        } else if (cur_col !== next_col) {
            // if moved up/down and ended up in different col then
            // it went through the wall
            return true;
        }

        return false;
    }

}
