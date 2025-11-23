import {
  Inject,
  Injectable,
  InternalServerErrorException,
  NotFoundException,
} from '@nestjs/common';
import { CreateVehicleDto } from './dto/create-vehicle.dto';
import { UpdateVehicleDto } from './dto/update-vehicle.dto';
import { ClientProxy } from '@nestjs/microservices';
import { InjectRepository } from '@nestjs/typeorm';
import { Repository } from 'typeorm';
import { Vehicle } from './entities/vehicle.entity';

@Injectable()
export class VehicleService {
  constructor(
    @Inject('MQTT_CLIENT')
    private readonly mqttClient: ClientProxy,
    @InjectRepository(Vehicle) private repo: Repository<Vehicle>,
  ) {}

  async getVehiclesSignedThisYear(year: number) {
    const vehicles = await this.repo
      .createQueryBuilder('vehicle')
      .where("strftime('%Y', vehicle.createdAt) = :year", {
        year: String(year),
      })
      .getMany();

    return vehicles;
  }
  // Serviço para emplacar veículos:
  async signupVehicle(createVehicleDto: CreateVehicleDto) {
    try {
      // Salvar o veículo no banco
      const signupedVehicle = this.repo.create(createVehicleDto);
      await this.repo.save(signupedVehicle);

      // Emitir evento MQTT para algum listener
      this.mqttClient.emit('events/vehicle/signup', signupedVehicle);

      return signupedVehicle;
    } catch (error) {
      console.log(error);
      throw new InternalServerErrorException(
        'Unexpected error on the vehicle signup',
      );
    }
  }

  async updateVehicleConductor(
    sign: string,
    updateVehicleDto: UpdateVehicleDto,
  ) {
    try {
      const updatedVehicle = await this.repo.update({ sign }, updateVehicleDto);

      this.mqttClient.emit('events/vehicle/updateConductor', updatedVehicle);
    } catch (error: unknown) {
      console.log(error);
      throw new InternalServerErrorException(
        'Unexpected error on the vehicle conductor update',
      );
    }
  }

  async calculateIPVA({ sign }: { sign: string }) {
    try {
      const vehicle = await this.repo.findOne({ where: { sign } });

      if (!vehicle) {
        throw new NotFoundException(
          `Vehicle with sign ${sign} does not exist on the database`,
        );
      }

      const ipva = vehicle.value * 0.02;

      return `IPVA of the vehcile with sign ${sign} is: R$${ipva}`;
    } catch (error: unknown) {
      console.log(error);
      throw new InternalServerErrorException(
        'Unexpected error on the IPVA calculation',
      );
    }
  }

  async findOneVehicle(sign: string) {
    const vehicle = await this.repo.findOne({ where: { sign } });

    if (!vehicle) {
      throw new NotFoundException(
        `Vehicle with sign ${sign} does not exist on the database`,
      );
    }

    return vehicle;
  }
}
