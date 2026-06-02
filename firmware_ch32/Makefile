CONTROLLER_DIR := controller
WORKER_DIR := worker

.PHONY: all controller worker flash_controller clean

all : controller worker

controller:
	@echo "Building Controller"
	$(MAKE) -C $(CONTROLLER_DIR)
worker:
	@echo "Building Worker"
	$(MAKE) -C $(WORKER_DIR)

flash_controller:
	$(MAKE) -C $(CONTROLLER_DIR) flash

flash_worker:
	$(MAKE) -C $(WORKER_DIR) flash

clean:
	$(MAKE) -C $(CONTROLLER_DIR) clean
	$(MAKE) -C $(WORKER_DIR) clean
