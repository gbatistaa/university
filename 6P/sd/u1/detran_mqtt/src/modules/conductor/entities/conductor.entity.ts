import { CommonEntity } from 'src/modules/common/entities/common.entity';
import { Column, Entity, Unique } from 'typeorm';

@Entity()
@Unique(['cpf'])
export class Conductor extends CommonEntity {
  @Column({ type: 'text', nullable: false })
  cpf: string;

  @Column({ type: 'text', nullable: false })
  name: string;
}
