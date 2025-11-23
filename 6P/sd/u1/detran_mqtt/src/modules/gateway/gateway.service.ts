import { Inject, Injectable } from '@nestjs/common';
import { CreateVehicleDto } from '../ms-vehicle/dto/create-vehicle.dto';
import { ClientProxy } from '@nestjs/microservices';
import { CreateConductorDto } from '../ms-conductor/dto/create-conductor.dto';
import { lastValueFrom } from 'rxjs';
import { UpdateConductorDto } from '../ms-conductor/dto/update-conductor.dto';
import { CreatePenaltyDto } from '../ms-penalty/dto/create-penalty.dto';

@Injectable()
export class GatewayService {
  constructor(
    @Inject('MQTT_CLIENT') private readonly mqttClient: ClientProxy,
  ) {}

  async getVehiclePenalties(payload: {
    sign: string;
    year: string;
  }): Promise<unknown> {
    const event = this.mqttClient.send(
      'commands/penalty/vehiclePenalties',
      payload,
    );

    return await lastValueFrom(event);
  }

  async getVehiclesSignedThisYear(year: string): Promise<unknown> {
    const event = this.mqttClient.send('commands/vehicle/signedThisYear', year);

    return await lastValueFrom(event);
  }

  async signupVehicle(createVehicleDto: CreateVehicleDto): Promise<unknown> {
    const event = this.mqttClient.send(
      'commands/vehicle/signup',
      createVehicleDto,
    );

    return await lastValueFrom(event);
  }

  signupConductor(createConductorDto: CreateConductorDto) {
    const event = this.mqttClient.emit(
      'commands/conductor/signup',
      createConductorDto,
    );

    return event;
  }

  launchPenalty(createPenaltyDto: CreatePenaltyDto) {
    const event = this.mqttClient.emit(
      'commands/penalty/launch',
      createPenaltyDto,
    );

    return event;
  }

  async updateVehicleConductor(updateVehiclePayload: {
    sign: string;
    updateVehicleDto: UpdateConductorDto;
  }): Promise<unknown> {
    const event = this.mqttClient.send(
      'commands/vehicle/updateConductor',
      updateVehiclePayload,
    );

    return await lastValueFrom(event);
  }
}
