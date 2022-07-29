const automationContainer = window["automation-container"];
const state = window.state;
const buttonsConfig = window.config.buttons;

const updateServerWithState = async (relayId, state) => {
  const response = await fetch(`/update?relayId=${relayId}&state=${state}`);
  init();
};

const handleSwitchClick = (e) => {
  const id = e.target.id;
  const dataset = e.target.dataset;
  const relayId = dataset.relayId;
  state[relayId] = state[relayId] === 0 ? 1 : 0;
  updateServerWithState(relayId, state[relayId]);
};

const createButton = (buttonMeta) => {
  const buttonContainer = document.createElement("section");
  buttonContainer.classList.add("button-container");
  const button = document.createElement("button");
  button.classList.add("button");
  button.id = buttonMeta.id;
  button.innerText = buttonMeta.display;
  button.onclick = handleSwitchClick;
  button.setAttribute("data-relay-id", buttonMeta.relayId);

  if (state[buttonMeta.relayId] === 0) {
    button.classList.add("off");
  } else {
    button.classList.add("on");
  }

  buttonContainer.appendChild(button);
  return buttonContainer;
};

const createButtonsContainer = () => {
  const container = document.createElement("div");
  container.classList.add("buttons-container", "center");
  return container;
};

const init = () => {
  automationContainer.innerHTML = "";
  const buttons = buttonsConfig.map(createButton);
  const buttonsContainer = createButtonsContainer();
  buttonsContainer.append(...buttons);
  automationContainer.append(buttonsContainer);
};

init();
