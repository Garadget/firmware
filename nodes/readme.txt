each node can generate and subscribe receive events
door object will have method to process incoming events and notify subscribers
door object will scan all nodes for new synchronous events (like door sensor)
nodes will call the processing loop in door to notify about asynchronous events (particle function call etc)

nodes:
- i2c tester

todo: use retained variables for state (maybe)
todo: debounce status alerts
todo: repeated night and timeout alerts
todo: mqtt by domain name if dns is not cloud based
todo: test switching protocols on/off

todo: handle the mqtt commands
todo: dead man message

todo: I2C interface
todo: test everything

todo: encrypt mqtt pass
