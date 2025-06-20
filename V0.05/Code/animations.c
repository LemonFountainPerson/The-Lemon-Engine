#include "animations.h"




int LoopParticleAnimation(Object *particle)
{
	if (particle->arg2 < 1 || particle->arg2 > 999)
	{
		particle->arg2 = 3;
	}

	if (particle->arg1 < 1 || particle->animationTick % particle->arg2 != 0)
	{
		return EXECUTION_UNNECESSARY;
	
	}

	int firstSprite = 1;
	int lastSprite = 1;
	int animateType = 1;

	// Add a new case whenever you make a new particle here
	switch(particle->currentAnimation)
	{
		case SPARKLE:
		lastSprite = 7;
		break;

		default:
		break;
	}

	if (lastSprite < firstSprite)
	{
		return INVALID_DATA;
	}

	if (particle->arg2 < 1 || particle->arg2 > 999)
	{
		particle->arg2 = 3;
	}


	DisplayData *particleDisplay = particle->ObjectDisplay;

	// Add a new case for specific animation sequences
	switch (animateType)
	{
		case 2:
		{
			int onOddLoop = 1 - ( particle->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particleDisplay->currentSprite++;
			}
			else
			{
				particleDisplay->currentSprite--;
			}

			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particleDisplay->currentSprite = lastSprite;
				}
				else
				{
					particleDisplay->currentSprite = firstSprite;
				}
			}

		} break;


		case -2:
		{
			int onOddLoop = 1 - ( particle->animationTick / ((lastSprite - firstSprite + 1) * particle->arg2) ) % 2;

			if (onOddLoop == 1)
			{
				particleDisplay->currentSprite--;
			}
			else
			{
				particleDisplay->currentSprite++;
			}


			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				if (onOddLoop == 1)
				{
					particleDisplay->currentSprite = firstSprite;
				}
				else
				{
					particleDisplay->currentSprite = lastSprite;
				}
			}

		} break;


		case -1:
		{
			particleDisplay->currentSprite--;

			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				particleDisplay->currentSprite = lastSprite;
			}

		} break;

		default:
		{
			particleDisplay->currentSprite++;

			if (particleDisplay->currentSprite > lastSprite || particleDisplay->currentSprite < firstSprite)
			{
				particle->arg1--;

				particleDisplay->currentSprite = firstSprite;
			}

		} break;
	}

	return 0;
}



int genericAnimationHandler(Object *inputObject)
{
	inputObject->animationTick++;

	int firstSprite = 1;
	int lastSprite = 1;
	int ticksBeforeNextFrame = 4;
	int animateType = 1;


	DisplayData *objectDisplay = inputObject->ObjectDisplay;

	switch (animateType)
	{
		case -1:
		{
			objectDisplay->currentSprite = lastSprite - (inputObject->animationTick / ticksBeforeNextFrame);

			if (objectDisplay->currentSprite < firstSprite || objectDisplay->currentSprite > lastSprite)
			{
				objectDisplay->currentSprite = lastSprite;
				inputObject->animationTick = 0;
			}
		} break;

		default:
		{
			objectDisplay->currentSprite = firstSprite + (inputObject->animationTick / ticksBeforeNextFrame);

			if (objectDisplay->currentSprite < firstSprite || objectDisplay->currentSprite > lastSprite)
			{
				objectDisplay->currentSprite = firstSprite;
				inputObject->animationTick = 0;
			}
		} break;
	}



}